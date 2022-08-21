// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
#include <stdio.h>
#include "usrinc.h"
#include "lcd-cfg.h"
#include "tslib.h"
#include "GUI.h"

#define TS_AVG_NUM  (10)

#define CMD_READ_X  (0xc0)
#define CMD_READ_Y  (0xd0)
#define CMD_READ_Z1 (0xe0)
#define CMD_READ_Z2 (0xf0)

static BOOL ns2009_read(uint8_t cmd, uint16_t* val);
static uint16_t ns2009_average(uint8_t buf[]);
static void (*pfPushSample)(int x, int y, int z) = 0;
static volatile uint8_t ts_pen_down = 0;
uint8_t const doTSCali = 1;   /* 1=启用RTP校准界面，0=关闭校准 */
static tslib_calib_t tslib_data = {
  .a = {-1014844, 8115, 61, -1460400, -47, 4909, 65536, 2835}
};

////////////////////////////////////////////////////////////////////////////////
static U64 __StackTouch[1024 / 8] MEM_PI_STACK;
static __task void __ThreadTouch(void);
static OS_TID _tid_touch = 0;

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: APP_CreateTouchService
//| 功能描述 |: 创建GUI线程并启动GUI服务
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
bool_t APP_CreateTouchService(void)
{
  _tid_touch = os_tsk_create_user(
                   __ThreadTouch,
                   TSK_PRIO_HIGHER,
                   __StackTouch,
                   sizeof(__StackTouch));

  if (_tid_touch == 0) {
    DBG_PUTS("Touch failed.\n");
    return false;
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __ThreadTouch
//| 功能描述 |: GUI线程
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static __task void __ThreadTouch(void)
{
  static uint16_t x, y, z1, z2;
  static uint16_t x_bak, y_bak;
  static tslib_point_t pnt;
  int z, zc;

  TSLIB_Init();
  sys_delay(200);
  tslib_update_matrix(tslib_data.a);
  puts("Touch Ready!");

  while (1) {
    if (!ns2009_read(CMD_READ_Z1, &z1)) {
      os_dly_wait(1);
      continue;
    }
    if (!ns2009_read(CMD_READ_Z2, &z2)) {
      os_dly_wait(1);
      continue;
    }
    if ((z1 > 50) && (z2 > z1)) {
      if (!ns2009_read(CMD_READ_X, &x)) {
        os_dly_wait(1);
        continue;
      }
      if (!ns2009_read(CMD_READ_Y, &y)) {
        os_dly_wait(1);
        continue;
      }
      if ((x_bak != 0xffff) && (y_bak != 0xffff)) {
        // 检查X坐标是否太离谱
        if (x_bak > x) {
          z = x_bak - x;
        } else {
          z = x - x_bak;
        }
        if (z > 500) {
          x_bak = 0xffff;
          continue;
        }
        // 检查Y坐标是否太离谱
        if (y_bak > y) {
          z = y_bak - y;
        } else {
          z = y - y_bak;
        }
        if (z > 400) {
          y_bak = 0xffff;
          continue;
        }
      }
      x_bak = x;
      y_bak = y;
      //printf("x=%04u,y=%04u,z1=%04u,z2=%04u\n", x, y, z1, z2);
      z = (((int)x * z2) / (int)z1 - x);
      printf("x=%04u,y=%04u,z=%04u\n", x, y, z);
      if ((z > 1000) && (z < 6000)) {
        if (pfPushSample) {
          pfPushSample(x, y, z);
        } else {
          zc = TSLIB_CalcZ(x, y);
          //printf("z=%d,zc=%d\n", z, zc);
          /* 1.25倍容差 */
          if ((z > zc - zc / 2) && (z < zc + zc / 2)) {
            /* 压力值许可范围判断 */
            TSLIB_CnvToLogPoint(&pnt, x, y);
            tslib_average_add(pnt.x, pnt.y);
          }
        }
      }
      sys_delay(2);

    } else {
      x_bak = y_bak = 0xffff;
      if (pfPushSample) {
        pfPushSample(-1, -1, -1);
      } else {
        tslib_average_clr();
      }
      sys_delay(10);
    }
  }
}

static uint16_t ns2009_average(uint8_t buf[])
{
  uint16_t vmin = 0xFFFF, vmax = 0, sum = 0, data;
  for (unsigned i = 0; i < TS_AVG_NUM; i++) {
    data = (buf[0] << 4) | (buf[1] >> 4);
    sum += data;
    if (data < vmin)
      vmin = data;
    if (data > vmax)
      vmax = data;
    buf += 2;
  }
  data = (sum - vmin - vmax) / (TS_AVG_NUM - 2);
  return data;
}

static BOOL ns2009_read(uint8_t cmd, uint16_t* val)
{
  // 使用静态变量，ns2009_read函数不可重入！
  static I2CMsg_TypeDef msgs[2 * TS_AVG_NUM];
  static uint8_t buf[2 + 2 * TS_AVG_NUM] = {0};
  I2CMsg_TypeDef* msg;
  int r;

  if (buf[0] == 0) {
    // 初始化配置
    for (int i = 0; i < TS_AVG_NUM; i++) {
      msg = &msgs[i * 2 + 0];
      msg->addr = 0x48;
      msg->flags = 0;
      msg->len = 1;
      msg->buf = &buf[0];

      msg = &msgs[i * 2 + 1];
      msg->addr = 0x48;
      msg->flags = I2C_M_RD;
      msg->len = 2;
      msg->buf = &buf[2 + i * 2];
    }
  }
  buf[0] = cmd;
  r = f1c100s_twi_xfer(TWI0_MID, msgs, COUNTOF(msgs));
  if (r != COUNTOF(msgs)) {
    //printf("r=%d\n", r);
    return FALSE;
  } else if (val) {
    *val = ns2009_average(&buf[2]);
  }
  return TRUE;
}

void tslib_clr_callback(void)
{
  /* 送入-1，被GUI解释为触摸笔抬起 */
  if (ts_pen_down) {
    GUI_TOUCH_StoreState(-1, -1);
    ts_pen_down = 0;
  }
}

void tslib_add_callback(int x, int y)
{
  /* 限定X坐标的取值范围 */
  if (x > LCD_PANEL_XSIZE - 1) {
    x = LCD_PANEL_XSIZE - 1;
  } else if (x < 0) {
    x = 0;
  }
  /* 限定Y坐标的取值范围 */
  if (y > LCD_PANEL_YSIZE - 1) {
    y = LCD_PANEL_YSIZE - 1;
  } else if (y < 0) {
    y = 0;
  }

  if (ts_pen_down == 0) {
    /* 触摸笔接触屏幕，发出点声音或什么... */
  }

  ts_pen_down = 1;
  GUI_TOUCH_StoreState(x, y);
}

void Touch_SetSampleCallback(void (*pf)(int, int, int))
{
  pfPushSample = pf;
}

void Touch_CacheTheSample(int id, uint32_t x, uint32_t y, uint32_t z, uint32_t lx, uint32_t ly)
{
  tslib_data.x[id]   = x;
  tslib_data.y[id]   = y;
  tslib_data.z[id]   = z;
  tslib_data.xfb[id] = lx;
  tslib_data.yfb[id] = ly;
}

bool_t Touch_MakeCalibrationData(void)
{
  if (perform_calibration(&tslib_data)) {
    tslib_update_matrix(tslib_data.a);
    for (int i = 0; i < TSC_SAVE_NUM; i++) {
      printf("%d,", tslib_data.a[i]);
    }
    putchar('\n');
    return TRUE;
  }
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////


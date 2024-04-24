#include <stdio.h>
#include <stdbool.h>
#include "usrinc.h"
#include "lvgl.h"
#include "framebuffer.h"
#include "f1c100s/reg-tcon.h"
#include "f1c100s-irq.h"
#include "target.h"
#include "mmu.h"
#include "io.h"

#include "demos/lv_demos.h"

#define XSIZE_PHYS    (480)
#define YSIZE_PHYS    (272)
#define DISP_BUF_SIZE (XSIZE_PHYS*YSIZE_PHYS)

typedef struct {
  int  scr_sizx;
  int  scr_sizy;
  int  pnt_x, pnt_y;
  bool mouse_down;
} lv_sim_context_t;

////////////////////////////////////////////////////////////////////////////////
static U64 __StackGUI[16384 / 8] MEM_PI_STACK;
static __task void __ThreadGUI(void);
static OS_TID _gui_tid = 0;

static framebuffer_t fb_f1c100s;
static void lcd_frame_irq(void)
{
  write32(F1C100S_TCON_BASE + 0x04, (1U << 31));
  MMU_TestCleanDCache();
}
static void _InitController(void)
{
  f1c100s_intc_set_isr(F1C100S_IRQ_LCD, lcd_frame_irq);
  fb_f1c100s_init(&fb_f1c100s);
  fb_f1c100s_present(&fb_f1c100s, 0);
  fb_f1c100s_setbl(&fb_f1c100s, 100);
  write32(F1C100S_TCON_BASE + 0x04, (1U << 31));
  f1c100s_intc_enable_irq(F1C100S_IRQ_LCD);
}
static bool lv_sim_init(lv_sim_context_t* ctx);
////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: APP_CreateGuiService
//| 功能描述 |: 创建GUI线程并启动GUI服务
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
bool_t APP_CreateGuiService(void)
{
  _gui_tid = os_tsk_create_user(
                 __ThreadGUI,
                 TSK_PRIO_LOLIMIT,
                 __StackGUI,
                 sizeof(__StackGUI));

  if (_gui_tid == 0) {
    DBG_PUTS("GUI failed.\n");
    return false;
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __ThreadGUI
//| 功能描述 |: GUI线程
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static __task void __ThreadGUI(void)
{
  static lv_sim_context_t context = {0};

  printf("RTX+LVGL\n");
  _InitController();
  context.scr_sizx = XSIZE_PHYS;
  context.scr_sizy = YSIZE_PHYS;
  // Initialize LVGL
  lv_init();
  lv_sim_init(&context);
  // 运行LVGL应用
#if LV_USE_DEMO_WIDGETS
  lv_demo_widgets();
#endif
#if LV_USE_DEMO_MUSIC
  lv_demo_music();
#endif
  while (true) {
    lv_task_handler();
    sys_delay(5);
  }
  //os_tsk_delete_self();
}

static void lv_sim_disp_flush_callback(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p);
static void lv_sim_pid_read_callback(lv_indev_drv_t* indev_drv, lv_indev_data_t* data);
static bool lv_sim_init(lv_sim_context_t* ctx)
{
  static lv_color_t buf1[DISP_BUF_SIZE];
  static lv_disp_draw_buf_t display_buffer;

  lv_disp_draw_buf_init(&display_buffer, buf1, NULL, DISP_BUF_SIZE);

  static lv_disp_drv_t display_driver;
  lv_disp_drv_init(&display_driver);
  display_driver.user_data = ctx;
  display_driver.hor_res = ctx->scr_sizx;
  display_driver.ver_res = ctx->scr_sizy;
  display_driver.draw_buf = &display_buffer;
  display_driver.flush_cb = lv_sim_disp_flush_callback;
  if (!lv_disp_drv_register(&display_driver)) {
    return false;
  }

  static lv_indev_drv_t pointer_driver;
  lv_indev_drv_init(&pointer_driver);
  pointer_driver.type = LV_INDEV_TYPE_POINTER;
  pointer_driver.user_data = ctx;
  pointer_driver.read_cb = lv_sim_pid_read_callback;
  if (!lv_indev_drv_register(&pointer_driver)) {
    return false;
  }

  return true;
}

static void lv_sim_disp_flush_callback(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p)
{
  lv_sim_context_t* ctx = (lv_sim_context_t*)disp_drv->user_data;
  uint32_t* disp_fb = fb_f1c100s_get_vram1();
  uint32_t pixel;
  for (int y = area->y1; y <= area->y2; ++y) {
    for (int x = area->x1; x <= area->x2; ++x) {
      pixel = ((color_p->ch.blue << 19) | (color_p->ch.green << 10) | (color_p->ch.red << 3));
      disp_fb[y * XSIZE_PHYS + x] = pixel;
      color_p++;
    }
  }
  lv_disp_flush_ready(disp_drv);
}

static void lv_sim_pid_read_callback(lv_indev_drv_t* indev_drv, lv_indev_data_t* data)
{
  lv_sim_context_t* ctx = (lv_sim_context_t*)indev_drv->user_data;

  data->state = (lv_indev_state_t)(ctx->mouse_down ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL);
  data->point.x = ctx->pnt_x;
  data->point.y = ctx->pnt_y;
}

uint32_t __LV_GetCustomTick(void)
{
  return sys_tick;
}


////////////////////////////////////////////////////////////////////////////////


// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
#include <stdint.h>
#include <stdio.h>
#include "f1c100s/reg-ccu.h"
#include "f1c100s/reg-debe.h"
#include "f1c100s/reg-defe.h"
#include "f1c100s/reg-dram.h"
#include "f1c100s/reg-tcon.h"
#include "f1c100s-gpio.h"
#include "f1c100s-irq.h"
#include "f1c100s-reset.h"
#include "target.h"
#include "io.h"
// Peripheral drivers
#include "gpio-f1c100s.h"
#include <RTL.h>


#ifndef __IO
#define __IO volatile
#endif

enum {
  I2C_STAT_BUS_ERROR  = 0x00,
  I2C_STAT_TX_START   = 0x08,
  I2C_STAT_TX_RSTART  = 0x10,
  I2C_STAT_TX_AW_ACK  = 0x18,
  I2C_STAT_TX_AW_NAK  = 0x20,
  I2C_STAT_TXD_ACK    = 0x28,
  I2C_STAT_TXD_NAK    = 0x30,
  I2C_STAT_LOST_ARB   = 0x38,
  I2C_STAT_TX_AR_ACK  = 0x40,
  I2C_STAT_TX_AR_NAK  = 0x48,
  I2C_STAT_RXD_ACK    = 0x50,
  I2C_STAT_RXD_NAK    = 0x58,
  I2C_STAT_IDLE       = 0xf8,
};

#define TWI0_REG_BASE   0x01C27000
#define TWI1_REG_BASE   0x01C27400
#define TWI2_REG_BASE   0x01C27800

typedef struct {
  __IO uint32_t ADDR;   // TWI Slave Address
  __IO uint32_t XADDR;  // TWI Extended Slave Address Register
  __IO uint32_t DATA;   // TWI Data Byte Register
  __IO uint32_t CNTR;   // TWI Control Register
  __IO uint32_t STAT;   // TWI Status Register
  __IO uint32_t CCR;    // TWI Clock Control Register
  __IO uint32_t SRST;   // TWI Software Reset Register
  __IO uint32_t EFR;    // TWI Enhance Feature Register
  __IO uint32_t LCR;    // TWI Line Control Register
} TWI_REG_TypeDef;

#define TWI0_REG  ((TWI_REG_TypeDef*)TWI0_REG_BASE)
#define TWI1_REG  ((TWI_REG_TypeDef*)TWI1_REG_BASE)
#define TWI2_REG  ((TWI_REG_TypeDef*)TWI2_REG_BASE)

typedef struct TWI_MOD TWI_MOD_TypeDef;
struct TWI_MOD {
  TWI_REG_TypeDef* regs;
  void (*init)(TWI_MOD_TypeDef* module);
  I2CMsg_TypeDef* msgs;
  uint32_t ccr_value;
  __IO OS_TID threadid;
  __IO uint8_t iic_sta;
};

static void twi0_init(TWI_MOD_TypeDef* module);
static void twi1_init(TWI_MOD_TypeDef* module);
static void twi2_init(TWI_MOD_TypeDef* module);
static void twi0_irq(void);
static void twi1_irq(void);
static void twi2_irq(void);
static void twi_irq_handler(TWI_MOD_TypeDef* module);
static void f1c100s_i2c_set_rate(TWI_MOD_TypeDef* module, uint32_t rate);
static TWI_MOD_TypeDef modules[3] = {
  [0] = {
    .regs = TWI0_REG,
    .init = twi0_init,
  },
  [1] = {
    .regs = TWI1_REG,
    .init = twi1_init,
  },
  [2] = {
    .regs = TWI2_REG,
    .init = twi2_init,
  },
};

static void twi0_irq(void)
{
  twi_irq_handler(&modules[0]);
}

static void twi1_irq(void)
{
  twi_irq_handler(&modules[1]);
}

static void twi2_irq(void)
{
  twi_irq_handler(&modules[2]);
}

static void twi0_init(TWI_MOD_TypeDef* module)
{
  uint32_t reg_addr, reg_value;

  // 开启时钟
  reg_addr = F1C100S_CCU_BASE + CCU_BUS_CLK_GATE2;
  reg_value = read32(reg_addr);
  reg_value |= (1U << 16);
  write32(reg_addr, reg_value);

  // 退出复位状态
  reg_addr = F1C100S_CCU_BASE + CCU_BUS_SOFT_RST2;
  reg_value = read32(reg_addr);
  reg_value |= (1U << 16);
  write32(reg_addr, reg_value);

  // 配置管脚功能
#ifdef PCB_TINY200V2
  gpio_f1c100s_set_cfg(&GPIO_PD, 12, 3);
  gpio_f1c100s_set_cfg(&GPIO_PD, 0, 3);
  gpio_f1c100s_set_pull(&GPIO_PD, 12, GPIO_PULL_UP);
  gpio_f1c100s_set_pull(&GPIO_PD, 0, GPIO_PULL_UP);
#else
  gpio_f1c100s_set_cfg(&GPIO_PE, 12, 3);
  gpio_f1c100s_set_cfg(&GPIO_PE, 11, 3);
  gpio_f1c100s_set_pull(&GPIO_PE, 12, GPIO_PULL_UP);
  gpio_f1c100s_set_pull(&GPIO_PE, 11, GPIO_PULL_UP);
#endif

  // 配置中断
  f1c100s_intc_set_isr(F1C100S_IRQ_I2C0, twi0_irq);
  f1c100s_intc_enable_irq(F1C100S_IRQ_I2C0);
}

static void twi1_init(TWI_MOD_TypeDef* module)
{
  uint32_t reg_addr, reg_value;

  // 开启时钟
  reg_addr = F1C100S_CCU_BASE + CCU_BUS_CLK_GATE2;
  reg_value = read32(reg_addr);
  reg_value |= (1U << 17);
  write32(reg_addr, reg_value);

  // 退出复位状态
  reg_addr = F1C100S_CCU_BASE + CCU_BUS_SOFT_RST2;
  reg_value = read32(reg_addr);
  reg_value |= (1U << 17);
  write32(reg_addr, reg_value);


  // 配置中断
  f1c100s_intc_set_isr(F1C100S_IRQ_I2C1, twi1_irq);
  f1c100s_intc_enable_irq(F1C100S_IRQ_I2C1);
}

static void twi2_init(TWI_MOD_TypeDef* module)
{
  uint32_t reg_addr, reg_value;

  // 开启时钟
  reg_addr = F1C100S_CCU_BASE + CCU_BUS_CLK_GATE2;
  reg_value = read32(reg_addr);
  reg_value |= (1U << 18);
  write32(reg_addr, reg_value);

  // 退出复位状态
  reg_addr = F1C100S_CCU_BASE + CCU_BUS_SOFT_RST2;
  reg_value = read32(reg_addr);
  reg_value |= (1U << 18);
  write32(reg_addr, reg_value);

  // 配置中断
  f1c100s_intc_set_isr(F1C100S_IRQ_I2C2, twi2_irq);
  f1c100s_intc_enable_irq(F1C100S_IRQ_I2C2);
}

static void twi_irq_handler(TWI_MOD_TypeDef* module)
{
  TWI_REG_TypeDef* regs = module->regs;
  if (regs->CNTR & (1 << 3)) {
    module->iic_sta = (uint8_t)regs->STAT;
    // 切勿在此处清除INT_FLAG标志！
    regs->CNTR &= ~(1 << 7);
    //printf("iic-s: %02x\n", module->iic_sta);
    isr_evt_set((1 << 0), module->threadid);
  }
}

static void f1c100s_i2c_set_rate(TWI_MOD_TypeDef* module, uint32_t rate)
{
  TWI_REG_TypeDef* regs = module->regs;
  uint32_t freq, delta, best = ~0UL;
  uint32_t tm = 5, tn = 0;
  uint32_t m, n;

  for (n = 0; n <= 7; n++) {
    for (m = 0; m <= 15; m++) {
      freq = CLK_APB / (10 * (m + 1) * (1 << n));
      if (rate >= freq) {
        delta = rate - freq;
        if (delta < best) {
          tm = m;
          tn = n;
          best = delta;
          if (best == 0) {
            goto _estb;
          }
        }
      }
    }
  }
_estb:
  //printf("iic:m=%u,n=%u\n", tm, tn);
  module->ccr_value = ((tm << 3) | (tn << 0));
  regs->CCR = module->ccr_value;
}


/*
* 初始化TWI模块
* @mid:    TWI模块编号[0,1,2]
* @freq:   CLK时钟频率
* @return: none
*/
void f1c100s_twi_init(uint8_t mid, uint32_t freq)
{
  TWI_MOD_TypeDef* module = &modules[mid];
  TWI_REG_TypeDef* regs = module->regs;

  if (module->init) {
    module->init(module);
  }
  regs->SRST = (1 << 0);
  while (regs->SRST & (1 << 0));
  f1c100s_i2c_set_rate(module, freq);
}

/*
* TWI传输
* @mid:    TWI模块编号[0,1,2]
* @msgs:   传输消息列表
* @num:    传输消息个数
* @return: 成功返回num，失败返回(-1)
*/
static uint8_t f1c100s_twi_wait_status(TWI_MOD_TypeDef* module);
static uint8_t f1c100s_twi_start(TWI_MOD_TypeDef* module);
static uint8_t f1c100s_twi_stop(TWI_MOD_TypeDef* module);
static uint8_t f1c100s_twi_send_data(TWI_MOD_TypeDef* module, uint8_t data);
static int f1c100s_twi_read(TWI_MOD_TypeDef* module);
static int f1c100s_twi_write(TWI_MOD_TypeDef* module);
int f1c100s_twi_xfer(uint8_t mid, I2CMsg_TypeDef msgs[], int num)
{
  TWI_MOD_TypeDef* module = &modules[mid];
  TWI_REG_TypeDef* regs = module->regs;
  int i, res;

  if ((msgs == 0) || (num <= 0)) {
    return 0;
  }

  module->iic_sta = 0;
  module->threadid = os_tsk_self();
  module->msgs = msgs;
  regs->CCR = module->ccr_value;
  regs->SRST = (1 << 0);
  while (regs->SRST & (1 << 0));
  regs->CNTR = (1 << 6);
  os_evt_clr(0xFFFF, module->threadid);

  // 发送START位
  if (f1c100s_twi_start(module) != I2C_STAT_TX_START) {
    regs->CNTR = (1 << 6);
    return 0;
  }

  for (i = 0; i < num; i++) {
    if ((i != 0) && !(module->msgs->flags & I2C_M_NOSTART)) {
      // 发送RESTART位
      if (f1c100s_twi_start(module) != I2C_STAT_TX_RSTART) {
        break;
      }
    }
    if (module->msgs->flags & I2C_M_RD) {
      res = f1c100s_twi_read(module);
    } else {
      res = f1c100s_twi_write(module);
    }
    module->msgs += 1;
    if (res < 0) {
      break;
    }
  }

  // 发送STOP位
  f1c100s_twi_stop(module);

  return i;
}

static uint8_t f1c100s_twi_wait_status(TWI_MOD_TypeDef* module)
{
  if (OS_R_EVT == os_evt_wait_or(0xFFFF, 10)) {
    return module->iic_sta;
  }
  return I2C_STAT_BUS_ERROR;
}

static uint8_t f1c100s_twi_start(TWI_MOD_TypeDef* module)
{
  TWI_REG_TypeDef* regs = module->regs;

  // (INT_EN|BUS_EN|M_STA)
  regs->CNTR = (0xc0 | (1 << 5));
  return f1c100s_twi_wait_status(module);
}

static uint8_t f1c100s_twi_stop(TWI_MOD_TypeDef* module)
{
  TWI_REG_TypeDef* regs = module->regs;
  uint32_t retries = 5;

  // (BUS_EN|M_STP)
  regs->CNTR = ((1 << 6) | (1 << 4));
  while (regs->CNTR & (1 << 4)) {
    if (--retries == 0) {
      break;
    }
    os_dly_wait(1);
  }
  return regs->STAT;
}

static uint8_t f1c100s_twi_send_data(TWI_MOD_TypeDef* module, uint8_t data)
{
  TWI_REG_TypeDef* regs = module->regs;

  regs->DATA = data;
  // (INT_EN|BUS_EN)
  regs->CNTR = (0xc0 | 0);
  return f1c100s_twi_wait_status(module);
}

static int f1c100s_twi_read(TWI_MOD_TypeDef* module)
{
  TWI_REG_TypeDef* regs = module->regs;
  unsigned len = module->msgs->len;
  uint8_t* p = module->msgs->buf;

  // 发送(ADDRESS|RW)
  if (f1c100s_twi_send_data(module, (uint8_t)((module->msgs->addr << 1) | 1)) != I2C_STAT_TX_AR_ACK) {
    return -1;
  }

  while (len > 0) {
    switch (module->iic_sta) {
      case I2C_STAT_TX_AR_ACK:
__read_next_byte:
        if (len == 1) {
          // (INT_EN|BUS_EN)
          regs->CNTR = (0xc0 | 0);
        } else {
          // (INT_EN|BUS_EN|A_ACK)
          regs->CNTR = (0xc0 | (1 << 2));
        }
        f1c100s_twi_wait_status(module);
        break;
      case I2C_STAT_RXD_ACK:
        *p++ = regs->DATA;
        len--;
        goto __read_next_byte;
      case I2C_STAT_RXD_NAK:
        *p++ = regs->DATA;
        len--;
        break;
      default:
        return -1;
    }
  }
  return 0;
}

static int f1c100s_twi_write(TWI_MOD_TypeDef* module)
{
  unsigned len = module->msgs->len;
  uint8_t* p = module->msgs->buf;

  // 发送(ADDRESS|0)
  if (f1c100s_twi_send_data(module, (uint8_t)(module->msgs->addr << 1)) != I2C_STAT_TX_AW_ACK) {
    return -1;
  }

  while (len > 0) {
    if (f1c100s_twi_send_data(module, *p++) != I2C_STAT_TXD_ACK) {
      return -1;
    }
    len--;
  }
  return 0;
}





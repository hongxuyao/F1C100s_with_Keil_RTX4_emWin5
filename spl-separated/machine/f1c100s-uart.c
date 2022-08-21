// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
#include <stdint.h>
#include <string.h>
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

#define UART0_REG_BASE      0x01C25000
#define UART1_REG_BASE      0x01C25400
#define UART2_REG_BASE      0x01C25800

#define UART_RBR_REG        0x00  // UART Receive Buffer Register
#define UART_THR_REG        0x00  // UART Transmit Holding Register
#define UART_DLL_REG        0x00  // UART Divisor Latch Low Register
#define UART_DLH_REG        0x04  // UART Divisor Latch High Register
#define UART_IER_REG        0x04  // UART Interrupt Enable Register
#define UART_IIR_REG        0x08  // UART Interrupt Identity Register
#define UART_FCR_REG        0x08  // UART FIFO Control Register
#define UART_LCR_REG        0x0C  // UART Line Control Register For Allwinner Tech Enthusiasts
#define UART_MCR_REG        0x10  // UART Modem Control Register
#define UART_LSR_REG        0x14  // UART Line Status Register
#define UART_MSR_REG        0x18  // UART Modem Status Register
#define UART_SCH_REG        0x1C  // UART Scratch Register
#define UART_USR_REG        0x7C  // UART Status Register
#define UART_TFL_REG        0x80  // UART Transmit FIFO Level Register
#define UART_RFL_REG        0x84  // UART Receive FIFO Level Register
#define UART_HSK_REG        0x88  // UART DMA Handshake Config Register
#define UART_HALT_REG       0xA4  // UART Halt TX Register
#define UART_DBG_DLL_REG    0xB0  // UART Debug DLL Register
#define UART_DBG_DLH_REG    0xB4  // UART Debug DLH Register


#define RX_FIFO_SIZ         1024
#if ((RX_FIFO_SIZ<2) || (RX_FIFO_SIZ&(RX_FIFO_SIZ-1)))
#error RX_FIFO_SIZ 必须是2的n次方，且不能为0也不能为1！
#else
#define RX_FIFO_MSK         (RX_FIFO_SIZ-1)
#endif

typedef struct UART_MOD UART_MOD_TypeDef;
struct UART_MOD {
  uint32_t regs_base;
  void (*init)(UART_MOD_TypeDef* module);
  void (*txEndCallback)(void);
  void (*rxCallback)(void);
  uint32_t baudrate;
  __IO OS_TID threadid;
  __IO bool_t isTxBusy;
  uint8_t const* pTxData;
  uint32_t txLength;
  __IO uint16_t in_offset;
  __IO uint16_t out_offset;
  uint8_t  rx_fifo[RX_FIFO_SIZ];
};

static void uart_init_regs(UART_MOD_TypeDef* module);
static void uart0_init(UART_MOD_TypeDef* module);
static void uart1_init(UART_MOD_TypeDef* module);
static void uart2_init(UART_MOD_TypeDef* module);
static void uart0_irq(void);
static void uart1_irq(void);
static void uart2_irq(void);
static void uart_irq_handler(UART_MOD_TypeDef* module);
static UART_MOD_TypeDef modules[3] = {
  [0] = {
    .regs_base = UART0_REG_BASE,
    .init = uart0_init,
    .baudrate = 115200,
  },
  [1] = {
    .regs_base = UART1_REG_BASE,
    .init = uart1_init,
    .baudrate = 115200,
  },
  [2] = {
    .regs_base = UART2_REG_BASE,
    .init = uart2_init,
    .baudrate = 115200,
  },
};

static void uart0_irq(void)
{
  uart_irq_handler(&modules[0]);
}

static void uart1_irq(void)
{
  uart_irq_handler(&modules[1]);
}

static void uart2_irq(void)
{
  uart_irq_handler(&modules[2]);
}

static void uart0_init(UART_MOD_TypeDef* module)
{
  uint32_t reg_addr, reg_value;

  // 开启时钟
  reg_addr = F1C100S_CCU_BASE + CCU_BUS_CLK_GATE2;
  reg_value = read32(reg_addr);
  reg_value |= (1U << 20);
  write32(reg_addr, reg_value);

  // 退出复位状态
  reg_addr = F1C100S_CCU_BASE + CCU_BUS_SOFT_RST2;
  reg_value = read32(reg_addr);
  reg_value |= (1U << 20);
  write32(reg_addr, reg_value);

  // 配置管脚功能
  gpio_f1c100s_set_cfg(&GPIO_PE, 0, 5);
  gpio_f1c100s_set_cfg(&GPIO_PE, 1, 5);
  gpio_f1c100s_set_pull(&GPIO_PE, 0, GPIO_PULL_UP);
  gpio_f1c100s_set_pull(&GPIO_PE, 1, GPIO_PULL_UP);

  // 配置串口模块寄存器
  uart_init_regs(module);

  // 配置中断
  f1c100s_intc_set_isr(F1C100S_IRQ_UART0, uart0_irq);
  f1c100s_intc_enable_irq(F1C100S_IRQ_UART0);
}

static void uart1_init(UART_MOD_TypeDef* module)
{
  uint32_t reg_addr, reg_value;

  // 开启时钟
  reg_addr = F1C100S_CCU_BASE + CCU_BUS_CLK_GATE2;
  reg_value = read32(reg_addr);
  reg_value |= (1U << 21);
  write32(reg_addr, reg_value);

  // 退出复位状态
  reg_addr = F1C100S_CCU_BASE + CCU_BUS_SOFT_RST2;
  reg_value = read32(reg_addr);
  reg_value |= (1U << 21);
  write32(reg_addr, reg_value);

  // 配置串口模块寄存器
  uart_init_regs(module);

  // 配置中断
  f1c100s_intc_set_isr(F1C100S_IRQ_UART1, uart1_irq);
  f1c100s_intc_enable_irq(F1C100S_IRQ_UART1);
}

static void uart2_init(UART_MOD_TypeDef* module)
{
  uint32_t reg_addr, reg_value;

  // 开启时钟
  reg_addr = F1C100S_CCU_BASE + CCU_BUS_CLK_GATE2;
  reg_value = read32(reg_addr);
  reg_value |= (1U << 22);
  write32(reg_addr, reg_value);

  // 退出复位状态
  reg_addr = F1C100S_CCU_BASE + CCU_BUS_SOFT_RST2;
  reg_value = read32(reg_addr);
  reg_value |= (1U << 22);
  write32(reg_addr, reg_value);

  // 配置串口模块寄存器
  uart_init_regs(module);

  // 配置中断
  f1c100s_intc_set_isr(F1C100S_IRQ_UART2, uart2_irq);
  f1c100s_intc_enable_irq(F1C100S_IRQ_UART2);
}

static void uart_irq_handler(UART_MOD_TypeDef* module)
{
  uint32_t addr, val, iir, nMax, index;
  uint8_t const* pTxData;
  void (*pfCallback)(void);

  addr = module->regs_base;
  iir = (read32(addr + UART_IIR_REG) & 0x0F);
  switch (iir) {
    case 0x06:  // receiver line status
      val = read32(addr + UART_LSR_REG);
      break;
    case 0x04:
    case 0x0c:
      val = read32(addr + UART_LSR_REG);
      index = module->in_offset;
      while (val & 0x01) {
        if (val & 0x80) {
          val = read32(addr + UART_RBR_REG);
        } else {
          val = read32(addr + UART_RBR_REG);
          // push to soft-fifo
          module->rx_fifo[(index++) & RX_FIFO_MSK] = (uint8_t)val;
        }
        val = read32(addr + UART_LSR_REG);
      }
      module->in_offset = index;
      pfCallback = module->rxCallback;
      if (pfCallback) {
        pfCallback();
      }
      break;
    case 0x02:  // THR empty
      if (module->txLength > 0) {
        nMax = (32 - (read32(addr + UART_TFL_REG) & 0x1F));
        if (nMax > module->txLength)
          nMax = module->txLength;
        pTxData = module->pTxData;
        for (unsigned i = 0; i < nMax; i++) {
          write32(addr + UART_THR_REG, pTxData[i]);
        }
        module->pTxData = &pTxData[nMax];
        module->txLength -= nMax;
      } else {
        val = read32(addr + UART_IER_REG);
        write32(addr + UART_IER_REG, (val & ~(1 << 1)));
        module->isTxBusy = 0;
        pfCallback = module->txEndCallback;
        if (pfCallback) {
          pfCallback();
        }
      }
      break;
    case 0x00: // modem status
      val = read32(addr + UART_MSR_REG);
      break;
    case 0x07: //busy detect
      val = read32(addr + UART_USR_REG);
      break;
  }
}

static void uart_init_regs(UART_MOD_TypeDef* module)
{
  uint32_t addr, val;

  /* 配置UART参数115200-8-1-0 */
  addr = module->regs_base;
  write32(addr + UART_IER_REG, 0x0);
  write32(addr + UART_FCR_REG, (2 << 6) | (1 << 4) | 7);
  write32(addr + UART_MCR_REG, 0x0);
  while (read32(addr + UART_USR_REG) & 1);
  write32(addr + UART_LCR_REG, 0x83);
  val = (6250000UL / module->baudrate);
  write32(addr + UART_DLL_REG, ((val >> 0) & 0xff));
  write32(addr + UART_DLH_REG, ((val >> 8) & 0xff));
  write32(addr + UART_LCR_REG, 0x03);
  write32(addr + UART_IER_REG, (1 << 2) | (1 << 0));
}

/*
* 初始化UART模块
* @mid:    UART模块编号[0,1,2]
* @freq:   串口波特率
* @return: none
*/
bool_t f1c100s_uart_init(uint8_t mid)
{
  UART_MOD_TypeDef* module = &modules[mid];

  if (module->init) {
    module->init(module);
    return TRUE;
  }
  return FALSE;
}

/*
* 读取UART接收FIFO中的数据
* @mid:    UART模块编号[0,1,2]
* @buf:    用于容纳所读数据的内存
* @max:    要读取的字节数
* @return: 已经成功读取的字节数
*/
int  f1c100s_uart_readFifo(uint8_t mid, uint8_t buf[], unsigned int max)
{
  UART_MOD_TypeDef* module = &modules[mid];
  unsigned int total, cpynum;
  uint32_t in_offset;

  in_offset = (module->in_offset & RX_FIFO_MSK);
  if (module->out_offset == in_offset) {
    /* 接收FIFO为空 */
    return 0;
  }

  total = (RX_FIFO_MSK & (in_offset - module->out_offset));
  if (total > max) {
    total = max;
  } else {
    max = total;
  }

  if (module->out_offset + max >= RX_FIFO_SIZ) {
    cpynum = (RX_FIFO_SIZ - module->out_offset);
    memcpy(buf, &module->rx_fifo[module->out_offset], cpynum);
    module->out_offset = 0;
    buf += cpynum;
    max -= cpynum;
  }

  if (max > 0) {
    memcpy(buf, &module->rx_fifo[module->out_offset], max);
    module->out_offset += max;
  }

  return total;
}

/*
* 丢弃UART的软件RX-FIFO中的数据
* @mid:    UART模块编号[0,1,2]
* @return: none
*/
void f1c100s_uart_clearfifo(uint8_t mid)
{
  UART_MOD_TypeDef* module = &modules[mid];
  CPU_SR_DECL;

  CPU_ENTER_CRITICAL();
  module->in_offset = module->out_offset = 0;
  CPU_EXIT_CRITICAL();
}

/*
* 设置UART通信波特率
* @mid:    UART模块编号[0,1,2]
* @baud:   波特率整数
* @return: none
*/
void f1c100s_uart_setBaudrate(uint8_t mid, uint32_t baud)
{
  UART_MOD_TypeDef* module = &modules[mid];

  if (__swap_byte(1, &module->isTxBusy))
    return;

  if (module->baudrate != baud) {
    module->baudrate = baud;
    uart_init_regs(module);
  }
  module->isTxBusy = 0;
}

/*
* 获取UART的通信波特率
* @mid:    UART模块编号[0,1,2]
* @return: none
*/
uint32_t f1c100s_uart_getBaudrate(uint8_t mid)
{
  UART_MOD_TypeDef* module = &modules[mid];

  return module->baudrate;
}

/*
* 启动UART异步发送
* @mid:    UART模块编号[0,1,2]
* @ptr:    指向要发送数据的内存【必须保持有效，直到发送完成】
* @len:    要发送的字节数
* @return: none
*/
void f1c100s_uart_asyncTx(uint8_t mid, const void* ptr, unsigned int len)
{
  UART_MOD_TypeDef* module = &modules[mid];
  uint8_t const* pTxData = ptr;
  uint32_t addr, nMax;
  CPU_SR_DECL;

  if ((ptr == 0) || (len <= 0))
    return;

  if (__swap_byte(1, &module->isTxBusy))
    return;

  addr = module->regs_base;
  nMax = (32 - (read32(addr + UART_TFL_REG) & 0x1F));
  if (nMax > len)
    nMax = len;
  for (unsigned i = 0; i < nMax; i++) {
    write32(addr + UART_THR_REG, pTxData[i]);
  }
  module->pTxData = &pTxData[nMax];
  module->txLength = (len - nMax);

  CPU_ENTER_CRITICAL();
  write32(addr + UART_IER_REG, (read32(addr + UART_IER_REG) | (1 << 1)));
  CPU_EXIT_CRITICAL();
}

/*
* 指定UART接收到数据回调函数
* @mid:    UART模块编号[0,1,2]
* @pf:     接收信号回调函数
* @return: none
*/
void f1c100s_uart_setRxCallback(uint8_t mid, void (*pf)(void))
{
  UART_MOD_TypeDef* module = &modules[mid];
  module->rxCallback = pf;
}

/*
* 指定UART发送完成的回调函数
* @mid:    UART模块编号[0,1,2]
* @pf:     发送完成回调函数
* @return: none
*/
void f1c100s_uart_setTxCallback(uint8_t mid, void (*pf)(void))
{
  UART_MOD_TypeDef* module = &modules[mid];
  module->txEndCallback = pf;
}

/*
* 检测UART是否发送完成
* @mid:    UART模块编号[0,1,2]
* @param:  none
* @return: true=发送完成；false=正在发送
*/
bool_t f1c100s_uart_isTxEnd(uint8_t mid)
{
  UART_MOD_TypeDef* module = &modules[mid];
  return !module->isTxBusy;
}






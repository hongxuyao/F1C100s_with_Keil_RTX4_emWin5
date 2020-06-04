#include <stdint.h>
#include "f1c100s/reg-ccu.h"
#include "io.h"

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

#define PIO_REG_BASE    0x01c20800
#define PA_OFFSET       0x00
#define PB_OFFSET       0x24
#define PC_OFFSET       0x48
#define PD_OFFSET       0x6C
#define PE_OFFSET       0x90
#define PF_OFFSET       0xB4

#ifdef PCB_TINY200V2

void sys_uart_init(void)
{
  uint32_t addr;
  uint32_t val;

  /* UART管脚配置PA2&PA3 */
  addr = PIO_REG_BASE + PA_OFFSET + 0x00;
  val = read32(addr);
  val &= ~((0x7 << 12) | (0x7 << 8));
  val |= ((0x5 << 12) | (0x5 << 8));
  write32(addr, val);

  /* 打开UART时钟源 */
  addr = F1C100S_CCU_BASE + CCU_BUS_CLK_GATE2;
  val = read32(addr);
  val |= (1 << 21);
  write32(addr, val);

  /* 撤销UART总线复位 */
  addr = F1C100S_CCU_BASE + CCU_BUS_SOFT_RST2;
  val = read32(addr);
  val |= (1 << 21);
  write32(addr, val);

  /* 配置UART参数115200-8-1-0 */
  addr = UART1_REG_BASE;
  write32(addr + UART_IER_REG, 0x0);
  write32(addr + UART_FCR_REG, 0xf7);
  write32(addr + UART_MCR_REG, 0x0);
  val = read32(addr + UART_LCR_REG);
  val |= (1 << 7);
  write32(addr + UART_LCR_REG, val);
  write32(addr + UART_DLL_REG, (54 & 0xff));        // APB=100Mhz时div=54
  write32(addr + UART_DLH_REG, ((54 >> 8) & 0xff));
  val = read32(addr + UART_LCR_REG);
  val &= ~(1 << 7);
  write32(addr + UART_LCR_REG, val);
  val = read32(addr + UART_LCR_REG);
  val &= ~0x3f;
  val |= ((0x3 << 0) | (0 << 2) | (0x0 << 3));
  write32(addr + UART_LCR_REG, val);
}

void sys_uart_putc(char c)
{
  uint32_t addr = UART1_REG_BASE;
  // Wait for transmit FIFO Not Full!
  while ((read32(addr + UART_USR_REG) & (1 << 1)) == 0);
  write32(addr + UART_THR_REG, c);
}

#else

void sys_uart_init(void)
{
  uint32_t addr;
  uint32_t val;

  /* UART管脚配置PE0&PE1 */
  addr = PIO_REG_BASE + PE_OFFSET + 0x00;
  val = read32(addr);
  val &= ~((0x7 << 4) | (0x7 << 0));
  val |= ((0x5 << 4) | (0x5 << 0));
  write32(addr, val);

  /* 打开UART时钟源 */
  addr = F1C100S_CCU_BASE + CCU_BUS_CLK_GATE2;
  val = read32(addr);
  val |= (1 << 20);
  write32(addr, val);

  /* 撤销UART总线复位 */
  addr = F1C100S_CCU_BASE + CCU_BUS_SOFT_RST2;
  val = read32(addr);
  val |= (1 << 20);
  write32(addr, val);

  /* 配置UART参数115200-8-1-0 */
  addr = UART0_REG_BASE;
  write32(addr + UART_IER_REG, 0x0);
  write32(addr + UART_FCR_REG, 0xf7);
  write32(addr + UART_MCR_REG, 0x0);
  val = read32(addr + UART_LCR_REG);
  val |= (1 << 7);
  write32(addr + UART_LCR_REG, val);
  write32(addr + UART_DLL_REG, (54 & 0xff));        // APB=100Mhz时div=54，则baud=115200
  write32(addr + UART_DLH_REG, ((54 >> 8) & 0xff));
  val = read32(addr + UART_LCR_REG);
  val &= ~(1 << 7);
  write32(addr + UART_LCR_REG, val);
  val = read32(addr + UART_LCR_REG);
  val &= ~0x3f;
  val |= ((0x3 << 0) | (0 << 2) | (0x0 << 3));
  write32(addr + UART_LCR_REG, val);
}

void sys_uart_putc(char c)
{
  uint32_t addr = UART0_REG_BASE;
  // Wait for transmit FIFO Not Full!
  while ((read32(addr + UART_USR_REG) & (1 << 1)) == 0);
  write32(addr + UART_THR_REG, c);
}

#endif

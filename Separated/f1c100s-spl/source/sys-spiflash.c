#include <stdint.h>
#include <string.h>
#include "reg-ccu.h"
#include "io.h"

#define SPI0_REG_BASE   0x01C05000
#define SPI1_REG_BASE   0x01C06000

enum {
  SPI_GCR = 0x04,
  SPI_TCR = 0x08,
  SPI_IER = 0x10,
  SPI_ISR = 0x14,
  SPI_FCR = 0x18,
  SPI_FSR = 0x1c,
  SPI_WCR = 0x20,
  SPI_CCR = 0x24,
  SPI_MBC = 0x30,
  SPI_MTC = 0x34,
  SPI_BCC = 0x38,
  SPI_TXD = 0x200,
  SPI_RXD = 0x300,
};

#define PIO_REG_BASE    0x01c20800
#define PA_OFFSET       0x00
#define PB_OFFSET       0x24
#define PC_OFFSET       0x48
#define PD_OFFSET       0x6C
#define PE_OFFSET       0x90
#define PF_OFFSET       0xB4

void sys_spi_flash_init(void)
{
  uint32_t addr;
  uint32_t val;

  /* Config GPIOC0, GPIOC1, GPIOC2 and GPIOC3 */
  addr = PIO_REG_BASE + PC_OFFSET + 0x00;
  val = read32(addr);
  val &= ~((7 << 0) | (7 << 4) | (7 << 8) | (7 << 12));
  val |= ((2 << 0) | (2 << 4) | (2 << 8) | (2 << 12));
  write32(addr, val);

  /* Open the spi0 bus gate */
  addr = F1C100S_CCU_BASE + CCU_BUS_CLK_GATE0;
  val = read32(addr);
  val |= (1 << 20);
  write32(addr, val);

  /* Deassert spi0 reset */
  addr = F1C100S_CCU_BASE + CCU_BUS_SOFT_RST0;
  val = read32(addr);
  val |= (1 << 20);
  write32(addr, val);

  /* Set spi clock rate control register, divided by 4 */
  addr = SPI0_REG_BASE;
  write32(addr + SPI_CCR, 0x00001001);

  /* Enable spi0 and do a soft reset */
  addr = SPI0_REG_BASE;
  val = read32(addr + SPI_GCR);
  val |= ((1UL << 31) | (1 << 7) | (1 << 1) | (1 << 0));
  write32(addr + SPI_GCR, val);
  while (read32(addr + SPI_GCR) & (1UL << 31));

  val = read32(addr + SPI_TCR);
  val &= ~(0x3 << 0);
  val |= (1 << 6) | (1 << 2);
  write32(addr + SPI_TCR, val);

  val = read32(addr + SPI_FCR);
  val |= ((1UL << 31) | (1 << 15));
  write32(addr + SPI_FCR, val);
}

void sys_spi_flash_exit(void)
{
  uint32_t addr = SPI0_REG_BASE;
  uint32_t val;

  /* Disable the spi0 controller */
  val = read32(addr + SPI_GCR);
  val &= ~((1 << 1) | (1 << 0));
  write32(addr + SPI_GCR, val);
}

static void sys_spi_select(void)
{
  uint32_t addr = SPI0_REG_BASE;
  uint32_t val;

  val = read32(addr + SPI_TCR);
  val &= ~((0x3 << 4) | (0x1 << 7));
  val |= ((0 & 0x3) << 4) | (0x0 << 7);
  write32(addr + SPI_TCR, val);
}

static void sys_spi_deselect(void)
{
  uint32_t addr = SPI0_REG_BASE;
  uint32_t val;

  val = read32(addr + SPI_TCR);
  val &= ~((0x3 << 4) | (0x1 << 7));
  val |= ((0 & 0x3) << 4) | (0x1 << 7);
  write32(addr + SPI_TCR, val);
}

static void sys_spi_write_txbuf(uint8_t* buf, int len)
{
  uint32_t addr = SPI0_REG_BASE;
  int i;

  if (!buf)
    len = 0;

  write32(addr + SPI_MTC, (len & 0xffffff));
  write32(addr + SPI_BCC, (len & 0xffffff));
  for (i = 0; i < len; ++i) {
    write8(addr + SPI_TXD, *buf++);
  }
}

static int sys_spi_transfer(void* txbuf, void* rxbuf, int len)
{
  uint32_t addr = SPI0_REG_BASE;
  int count = len;
  uint8_t* tx = txbuf;
  uint8_t* rx = rxbuf;
  uint8_t val;
  int n, i;

  while (count > 0) {
    n = (count <= 64) ? count : 64;
    write32(addr + SPI_MBC, n);
    sys_spi_write_txbuf(tx, n);
    write32(addr + SPI_TCR, read32(addr + SPI_TCR) | (1UL << 31));

    while ((read32(addr + SPI_FSR) & 0xff) < n);
    for (i = 0; i < n; i++) {
      val = read8(addr + SPI_RXD);
      if (rx) {
        *rx++ = val;
      }
    }

    if (tx)
      tx += n;
    count -= n;
  }
  return len;
}

static int sys_spi_write_then_read(void* txbuf, int txlen, void* rxbuf, int rxlen)
{
  if (sys_spi_transfer(txbuf, NULL, txlen) != txlen)
    return -1;
  if (sys_spi_transfer(NULL, rxbuf, rxlen) != rxlen)
    return -1;
  return 0;
}

void sys_spi_flash_read(uint32_t addr, void* buf, int count)
{
  uint8_t tx[4];

  tx[0] = 0x03;
  tx[1] = (uint8_t)(addr >> 16);
  tx[2] = (uint8_t)(addr >> 8);
  tx[3] = (uint8_t)(addr >> 0);
  sys_spi_select();
  sys_spi_write_then_read(tx, 4, buf, count);
  sys_spi_deselect();
}


// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
#include <stdint.h>
#include "crc.h"

extern void loop_delay(uint32_t nloops);
extern void sys_clock_init(void);
extern void sys_dram_init(void);
extern void sys_uart_init(void);
extern void sys_spi_flash_init(void);
extern void sys_spi_flash_read(uint32_t addr, void* buf, int count);
extern void sys_spi_flash_exit(void);
extern void sys_uart_putc(char c);
extern void return_to_fel(void);
extern void jump_to_uboot(uint32_t entry);
extern void sys_mmu_init(void);
extern void mmu_clean_dcache(void);

#define uswap_32(x) \
  ((((x) & 0xff000000) >> 24) | \
   (((x) & 0x00ff0000) >>  8) | \
   (((x) & 0x0000ff00) <<  8) | \
   (((x) & 0x000000ff) << 24))

typedef uint32_t __be32;  /* 表示大端字节序的整数 */

typedef struct {
  __be32    ih_magic;     /* Image Header Magic Number  */
  __be32    ih_hcrc;      /* Image Header CRC Checksum  */
  __be32    ih_time;      /* Image Creation Timestamp */
  __be32    ih_size;      /* Image Data Size    */
  __be32    ih_load;      /* Data  Load  Address    */
  __be32    ih_ep;        /* Entry Point Address    */
  __be32    ih_dcrc;      /* Image Data CRC Checksum  */
  uint8_t   ih_os;        /* Operating System   */
  uint8_t   ih_arch;      /* CPU architecture   */
  uint8_t   ih_type;      /* Image Type     */
  uint8_t   ih_comp;      /* Compression Type   */
  uint8_t   ih_name[32];  /* Image Name   */
} image_header_t;

static void spl_print32(uint32_t val)
{
  char hex[] = "0123456789abcdef";
  sys_uart_putc(hex[0x0f & (val >> 28)]);
  sys_uart_putc(hex[0x0f & (val >> 24)]);
  sys_uart_putc(hex[0x0f & (val >> 20)]);
  sys_uart_putc(hex[0x0f & (val >> 16)]);
  sys_uart_putc(hex[0x0f & (val >> 12)]);
  sys_uart_putc(hex[0x0f & (val >> 8)]);
  sys_uart_putc(hex[0x0f & (val >> 4)]);
  sys_uart_putc(hex[0x0f & (val >> 0)]);
}

static void spl_prints(char const s[], uint32_t len)
{
  if (len == 0)
    len = ~0;

  while (*s && len) {
    sys_uart_putc(*s++);
    len--;
  }
}

void spl_main(void)
{
  image_header_t uboot;
  uint32_t load_addr;
  uint32_t load_size;
  uint32_t dcrc;
  // 配置时钟
  sys_clock_init();
  // 配置DDR
  sys_dram_init();
  // 配置UART模块
  sys_uart_init();
  // 配置页表并启用MMU
  sys_mmu_init();
  // 检查SPL的加载模式
  if (*(uint8_t*)0x58 == 1) {
    // FEL模式
    spl_prints("Boot to FEL mode\r\n", 0);
    spl_prints("entry=", 0);
    spl_print32(*(uint32_t*)0x54);
    spl_prints(", sp=", 0);
    spl_print32(*(uint32_t*)0x40);
    spl_prints(", lr=", 0);
    spl_print32(*(uint32_t*)0x44);
    spl_prints(".\r\n", 0);
    return_to_fel();
    return;
  }
  // 初始化SPI模块
  sys_spi_flash_init();
  spl_prints("Boot to SPI mode\r\n", 0);
  spl_prints("entry=", 0);
  spl_print32(*(uint32_t*)0x54);
  spl_prints(", sp=", 0);
  spl_print32(*(uint32_t*)0x40);
  spl_prints(", lr=", 0);
  spl_print32(*(uint32_t*)0x44);
  spl_prints(".\r\n", 0);
  sys_spi_flash_read(0x8000, &uboot, sizeof(uboot));
  if (uboot.ih_magic == uswap_32(0x27051956)) {
    spl_prints((char*)uboot.ih_name, 32);
    sys_uart_putc('\r');
    sys_uart_putc('\n');
    load_addr = uswap_32(uboot.ih_load);
    load_size = uswap_32(uboot.ih_size);
    spl_prints("Load 0x", 0);
    spl_print32(load_size);
    spl_prints(" bytes @0x", 0);
    spl_print32(load_addr);
__read_again:
    sys_spi_flash_read(0x8040, (void*)load_addr, load_size);
    mmu_clean_dcache();
    dcrc = crc32(0, (void*)load_addr, load_size);
    if (dcrc != uswap_32(uboot.ih_dcrc)) {
      spl_prints(" FAIL.\r\nLoad again", 0);
      goto __read_again;
    }
    sys_spi_flash_exit();
    spl_prints(" OK.\r\n", 0);
    load_addr = uswap_32(uboot.ih_ep);
    spl_prints("Jump to uboot @0x", 0);
    spl_print32(load_addr);
    sys_uart_putc('\r');
    sys_uart_putc('\n');
    jump_to_uboot(load_addr);
    return;
  }
  sys_spi_flash_exit();
}



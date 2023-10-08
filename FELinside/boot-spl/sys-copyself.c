/*
 * sys-copyself.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <stdint.h>
#include "arm32.h"

extern void return_to_fel(void);
extern void sys_mmu_init(void);
extern void sys_uart_putc(char c);
extern void sys_spi_flash_init(void);
extern void sys_spi_flash_exit(void);
extern void sys_spi_flash_read(int addr, void* buf, int count);
extern void mmu_clean_dcache(void);

void sys_copyself(uint32_t region_info[4])
{
  void* mem;
  uint32_t size;

  if (*(uint32_t volatile*)0x58 == 1) {
    // Boot by FEL
    sys_uart_putc('B');
    sys_uart_putc('o');
    sys_uart_putc('o');
    sys_uart_putc('t');
    sys_uart_putc(' ');
    sys_uart_putc('t');
    sys_uart_putc('o');
    sys_uart_putc(' ');
    sys_uart_putc('F');
    sys_uart_putc('E');
    sys_uart_putc('L');
    sys_uart_putc(' ');
    sys_uart_putc('m');
    sys_uart_putc('o');
    sys_uart_putc('d');
    sys_uart_putc('e');
    sys_uart_putc('\r');
    sys_uart_putc('\n');
    mem = (void*)region_info[2];
    size = (region_info[3] - region_info[2]);
    sys_mmu_init();
    for (uint32_t i = 0; i < size; i += 4) {
      *(uint32_t volatile*)mem = *(uint32_t*)(0x00 + i);
      mem = ((uint32_t*)mem) + 1;
    }
    mmu_clean_dcache();
    return_to_fel();
  } else {
    // Boot by BROM
    sys_uart_putc('B');
    sys_uart_putc('o');
    sys_uart_putc('o');
    sys_uart_putc('t');
    sys_uart_putc(' ');
    sys_uart_putc('t');
    sys_uart_putc('o');
    sys_uart_putc(' ');
    sys_uart_putc('S');
    sys_uart_putc('P');
    sys_uart_putc('I');
    sys_uart_putc(' ');
    sys_uart_putc('m');
    sys_uart_putc('o');
    sys_uart_putc('d');
    sys_uart_putc('e');
    sys_uart_putc('\r');
    sys_uart_putc('\n');
    sys_mmu_init();
    sys_spi_flash_init();
    // 先读取SPL部分，注意SPL不能大于16KB！因为TTB放在0x80004000处
    sys_spi_flash_read(0x0000, (void*)region_info[2], (region_info[3] - region_info[2]));
    // 再读取运行在0x80008000地址开始的APP部分
    sys_spi_flash_read(0x8000, (void*)region_info[0], (region_info[1] - region_info[0]));
    sys_spi_flash_exit();
    mmu_clean_dcache();
  }
}


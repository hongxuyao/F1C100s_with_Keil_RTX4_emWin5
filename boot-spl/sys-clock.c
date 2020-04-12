/*
 * sys-clock.c
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

//#include <xboot.h>
#include <stdint.h>
#include "f1c100s/reg-ccu.h"
#include "io.h"

static void sdelay(int loops)
{
loop_again:
  __asm volatile {
    SUBS loops, loops, #1
    BNE loop_again
  }
}

static void wait_pll_stable(uint32_t base)
{
  uint32_t rval = 0;
  uint32_t time = 0xffff;

  do {
    rval = read32(base);
    time--;
  } while (time && !(rval & (1 << 28)));
}

static void clock_set_pll_cpu(uint32_t clk)
{
  uint32_t n, k, m, p;
  uint32_t rval = 0;
  uint32_t div = 0;

  if (clk > 720000000)
    clk = 720000000;

  if ((clk % 24000000) == 0) {
    div = clk / 24000000;
    n = div - 1;
    k = 0;
    m = 0;
    p = 0;
  } else if ((clk % 12000000) == 0) {
    m = 1;
    div = clk / 12000000;
    if ((div % 3) == 0)
      k = 2;
    else if ((div % 4) == 0)
      k = 3;
    else
      k = 1;
    n = (div / (k + 1)) - 1;
    p = 0;
  } else {
    div = clk / 24000000;
    n = div - 1;
    k = 0;
    m = 0;
    p = 0;
  }

  // The PLL output = (24MHz*N*K)/(M*P).
  rval = read32(F1C100S_CCU_BASE + CCU_PLL_CPU_CTRL);
  rval &= ~((0x3 << 16) | (0x1f << 8) | (0x3 << 4) | (0x3 << 0));
  rval |= ((1U << 31) | (p << 16) | (n << 8) | (k << 4) | m);
  write32(F1C100S_CCU_BASE + CCU_PLL_CPU_CTRL, rval);
  wait_pll_stable(F1C100S_CCU_BASE + CCU_PLL_CPU_CTRL);
}

void sys_clock_init(void)
{
  uint32_t val;

  // 设置PLL和CPU-PLL的锁定延迟时间
  write32(F1C100S_CCU_BASE + CCU_PLL_STABLE_TIME0, 0x1ff);
  write32(F1C100S_CCU_BASE + CCU_PLL_STABLE_TIME1, 0x1ff);

  // 先把CPU时钟源设置为24Mhz晶振
  val = read32(F1C100S_CCU_BASE + CCU_CPU_CFG);
  val &= ~(0x3 << 16);
  val |= (0x1 << 16);
  write32(F1C100S_CCU_BASE + CCU_CPU_CFG, val);
  sdelay(100);

  // 设置PLL_VIDEO(N=66,M=8)
  write32(F1C100S_CCU_BASE + CCU_PLL_VIDEO_CTRL, 0x81004107);
  sdelay(100);
  // 设置PLL_PERIPH(N=25,M=1) to 600Mhz
  write32(F1C100S_CCU_BASE + CCU_PLL_PERIPH_CTRL, 0x80041800);
  sdelay(100);
  // 设置AHB/APB/HCLKC的工作频率
#define HCLKC_DIV (0)         // HCLKC=CPUCLK/1
#define AHB_CLK_SRC_SEL (3)   // AHB source is (PLL_PERIPH/AHB_PRE_DIV)
#define APB_CLK_RATIO (1)     // APB_CLK=AHB_CLK/2=100Mhz
#define AHB_PRE_DIV (2)       // AHB_source=600/3=200Mhz
#define AHB_CLK_DIV_RATIO (0) // AHB_CLK=AHB_source/1=200Mhz
  write32(F1C100S_CCU_BASE + CCU_AHB_APB_CFG, (
              (HCLKC_DIV << 16) |
              (AHB_CLK_SRC_SEL << 12) |
              (APB_CLK_RATIO << 8) |
              (AHB_PRE_DIV << 6) |
              (AHB_CLK_DIV_RATIO << 4)
          ));
  sdelay(100);
  // 开启DE的front-end和back-end的DCLK门？
  val = read32(F1C100S_CCU_BASE + CCU_DRAM_CLK_GATE);
  val |= (0x1 << 26) | (0x1 << 24);
  write32(F1C100S_CCU_BASE + CCU_DRAM_CLK_GATE, val);
  sdelay(100);
  // 配置PLL_CPU的值
  clock_set_pll_cpu(720000000);
  // 把CPU时钟源设置为PLL_CPU
  val = read32(F1C100S_CCU_BASE + CCU_CPU_CFG);
  val &= ~(0x3 << 16);
  val |= (0x2 << 16);
  write32(F1C100S_CCU_BASE + CCU_CPU_CFG, val);
  sdelay(100);
}

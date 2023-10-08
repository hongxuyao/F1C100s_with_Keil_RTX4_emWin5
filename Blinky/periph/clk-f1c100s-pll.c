/*
 * driver/clk_pll-f1c100s-pll.c
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
#include <stddef.h>
#include "clk-f1c100s-pll.h"
#include "f1c100s/reg-ccu.h"
#include "io.h"

const clk_pll_t clk_hosc = {
  .virt = F1C100S_CCU_BASE,
  .parent = NULL,
  .channel = -1,
  .prate = 24000000,
};

void clk_f1c100s_pll_set_parent(clk_pll_t* clk_pll, const char* pname)
{
}

const clk_pll_t* clk_f1c100s_pll_get_parent(clk_pll_t* clk_pll)
{
  return clk_pll->parent;
}

void clk_f1c100s_pll_set_enable(clk_pll_t* clk_pll, bool enable)
{
  uint32_t r = 0;

  switch (clk_pll->channel) {
    case 0:
      r = read32(clk_pll->virt + CCU_PLL_CPU_CTRL);
      if (enable)
        r |= (1UL << 31);
      else
        r &= ~(1UL << 31);
      write32(clk_pll->virt + CCU_PLL_CPU_CTRL, r);
      break;

    case 1:
      r = read32(clk_pll->virt + CCU_PLL_AUDIO_CTRL);
      if (enable)
        r |= (1UL << 31);
      else
        r &= ~(1UL << 31);
      write32(clk_pll->virt + CCU_PLL_AUDIO_CTRL, r);
      break;

    case 2:
      r = read32(clk_pll->virt + CCU_PLL_VIDEO_CTRL);
      if (enable)
        r |= (1UL << 31);
      else
        r &= ~(1UL << 31);
      write32(clk_pll->virt + CCU_PLL_VIDEO_CTRL, r);
      break;

    case 3:
      r = read32(clk_pll->virt + CCU_PLL_VE_CTRL);
      if (enable)
        r |= (1UL << 31);
      else
        r &= ~(1UL << 31);
      write32(clk_pll->virt + CCU_PLL_VE_CTRL, r);
      break;

    case 4:
      r = read32(clk_pll->virt + CCU_PLL_DDR_CTRL);
      if (enable)
        r |= (1UL << 31);
      else
        r &= ~(1UL << 31);
      write32(clk_pll->virt + CCU_PLL_DDR_CTRL, r);
      break;

    case 5:
      r = read32(clk_pll->virt + CCU_PLL_PERIPH_CTRL);
      if (enable)
        r |= (1UL << 31);
      else
        r &= ~(1UL << 31);
      write32(clk_pll->virt + CCU_PLL_PERIPH_CTRL, r);
      break;

    default:
      break;
  }
}

bool clk_f1c100s_pll_get_enable(clk_pll_t* clk_pll)
{
  uint32_t r = 0;

  switch (clk_pll->channel) {
    case 0:
      r = read32(clk_pll->virt + CCU_PLL_CPU_CTRL);
      break;

    case 1:
      r = read32(clk_pll->virt + CCU_PLL_AUDIO_CTRL);
      break;

    case 2:
      r = read32(clk_pll->virt + CCU_PLL_VIDEO_CTRL);
      break;

    case 3:
      r = read32(clk_pll->virt + CCU_PLL_VE_CTRL);
      break;

    case 4:
      r = read32(clk_pll->virt + CCU_PLL_DDR_CTRL);
      break;

    case 5:
      r = read32(clk_pll->virt + CCU_PLL_PERIPH_CTRL);
      break;

    default:
      break;
  }
  return (r & (1UL << 31)) ? true : false;
}

void clk_f1c100s_pll_set_rate(clk_pll_t* clk_pll, uint64_t prate, uint64_t rate)
{
}

uint64_t clk_f1c100s_pll_get_rate(clk_pll_t* clk_pll, uint64_t prate)
{
  uint32_t r, n, k, m, p;
  uint64_t rate = 0;

  switch (clk_pll->channel) {
    case 0:
      r = read32(clk_pll->virt + CCU_PLL_CPU_CTRL);
      n = ((r >> 8) & 0x1f) + 1;
      k = ((r >> 4) & 0x3) + 1;
      m = ((r >> 0) & 0x3) + 1;
      p = ((r >> 16) & 0x3) + 1;
      rate = (uint64_t)((prate * n * k) / (m * p));
      break;

    case 1:
      r = read32(clk_pll->virt + CCU_PLL_AUDIO_CTRL);
      if (r & (1 << 24))
        n = ((r >> 8) & 0xf) + 1;
      else
        n = ((r >> 8) & 0x7f) + 1;
      m = ((r >> 0) & 0x1f) + 1;
      rate = (uint64_t)((prate * n * 2) / m);
      break;

    case 2:
      r = read32(clk_pll->virt + CCU_PLL_VIDEO_CTRL);
      if (r & (1 << 24)) {
        n = ((r >> 8) & 0x7f) + 1;
        m = ((r >> 0) & 0xf) + 1;
        rate = (uint64_t)((prate * n) / m);
      } else {
        if (r & (1 << 25))
          rate = 297 * 1000 * 1000;
        else
          rate = 270 * 1000 * 1000;
      }
      break;

    case 3:
      r = read32(clk_pll->virt + CCU_PLL_VE_CTRL);
      if (r & (1 << 24)) {
        n = ((r >> 8) & 0x7f) + 1;
        m = ((r >> 0) & 0xf) + 1;
        rate = (uint64_t)((prate * n) / m);
      } else {
        if (r & (1 << 25))
          rate = 297 * 1000 * 1000;
        else
          rate = 270 * 1000 * 1000;
      }
      break;

    case 4:
      r = read32(clk_pll->virt + CCU_PLL_DDR_CTRL);
      n = ((r >> 8) & 0x1f) + 1;
      k = ((r >> 4) & 0x3) + 1;
      m = ((r >> 0) & 0x3) + 1;
      rate = (uint64_t)((prate * n * k) / m);
      break;

    case 5:
      r = read32(clk_pll->virt + CCU_PLL_PERIPH_CTRL);
      n = ((r >> 8) & 0x1f) + 1;
      k = ((r >> 4) & 0x3) + 1;
      m = ((r >> 0) & 0x3) + 1;
      p = ((r >> 16) & 0x3) + 1;
      rate = (uint64_t)((prate * n * k) / p);
      break;

    default:
      break;
  }

  return rate;
}


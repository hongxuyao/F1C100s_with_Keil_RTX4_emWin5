/*
 * driver/reset-f1c100s.c
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

#include "reset-f1c100s.h"
#include "io.h"

void reset_f1c100s_assert(reset_f1c100s_t* reset, int32_t offset)
{
  uint32_t val;

  if (offset >= reset->nreset)
    return;

  val = read32(reset->virt);
  val &= ~(1 << offset);
  write32(reset->virt, val);
}

void reset_f1c100s_deassert(reset_f1c100s_t* reset, int32_t offset)
{
  uint32_t val;

  if (offset >= reset->nreset)
    return;

  val = read32(reset->virt);
  val |= (1 << offset);
  write32(reset->virt, val);
}

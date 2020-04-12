/*
 * driver/gpio-f1c100s.c
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

#include "gpio-f1c100s.h"
#include "io.h"

const gpio_port_t GPIO_PA = {
  .virt = 0x01c20800,
  .base = 0,
  .ngpio  = 4,
  .oirq = 0
};

const gpio_port_t GPIO_PB = {
  .virt = 0x01c20824,
  .base = 0,
  .ngpio  = 4,
  .oirq = 0
};

const gpio_port_t GPIO_PC = {
  .virt = 0x01c20848,
  .base = 0,
  .ngpio  = 4,
  .oirq = 0
};

const gpio_port_t GPIO_PD = {
  .virt = 0x01c2086c,
  .base = 0,
  .ngpio  = 22,
  .oirq = 0
};

const gpio_port_t GPIO_PE = {
  .virt = 0x01c20890,
  .base = 0,
  .ngpio  = 13,
  .oirq = 0
};

const gpio_port_t GPIO_PF = {
  .virt = 0x01c208b4,
  .base = 0,
  .ngpio  = 6,
  .oirq = 0
};

enum {
  GPIO_CFG0 = 0x00,
  GPIO_CFG1 = 0x04,
  GPIO_CFG2 = 0x08,
  GPIO_CFG3 = 0x0c,
  GPIO_DAT  = 0x10,
  GPIO_DRV0 = 0x14,
  GPIO_DRV1 = 0x18,
  GPIO_PUL0 = 0x1c,
  GPIO_PUL1 = 0x20,
};

void gpio_f1c100s_set_cfg(const gpio_port_t* port, int32_t pin, int32_t cfg)
{
  uint32_t addr;
  uint32_t val;

  if (pin >= port->ngpio)
    return;

  addr = port->virt + GPIO_CFG0 + ((pin >> 3) << 2);
  val = read32(addr);
  val &= ~(0xf << ((pin & 0x7) << 2));
  val |= ((cfg & 0x7) << ((pin & 0x7) << 2));
  write32(addr, val);
}

int32_t gpio_f1c100s_get_cfg(const gpio_port_t* port, int32_t pin)
{
  uint32_t addr;
  uint32_t val;

  if (pin >= port->ngpio)
    return 0;

  addr = port->virt + GPIO_CFG0 + ((pin >> 3) << 2);
  val = (read32(addr) >> ((pin & 0x7) << 2)) & 0x7;
  return val;
}

void gpio_f1c100s_set_pull(const gpio_port_t* port, int32_t pin, gpio_pull_t pull)
{
  uint32_t addr;
  uint32_t val, v;

  if (pin >= port->ngpio)
    return;

  switch (pull) {
    case GPIO_PULL_UP:
      v = 0x1;
      break;

    case GPIO_PULL_DOWN:
      v = 0x2;
      break;

    case GPIO_PULL_NONE:
      v = 0x0;
      break;

    default:
      v = 0x0;
      break;
  }

  addr = port->virt + GPIO_PUL0 + ((pin >> 4) << 2);
  val = read32(addr);
  val &= ~(v << ((pin & 0xf) << 1));
  val |= (v << ((pin & 0xf) << 1));
  write32(addr, val);
}

gpio_pull_t gpio_f1c100s_get_pull(const gpio_port_t* port, int32_t pin)
{
  uint32_t addr;
  uint32_t v = 0;

  if (pin >= port->ngpio)
    return GPIO_PULL_NONE;

  addr = port->virt + GPIO_PUL0 + ((pin >> 4) << 2);
  v = (read32(addr) >> ((pin & 0xf) << 1)) & 0x3;

  switch (v) {
    case 0:
      return GPIO_PULL_NONE;
    case 1:
      return GPIO_PULL_UP;
    case 2:
      return GPIO_PULL_DOWN;
    default:
      break;
  }
  return GPIO_PULL_NONE;
}

void gpio_f1c100s_set_drv(const gpio_port_t* port, int32_t pin, gpio_drv_t drv)
{
  uint32_t addr;
  uint32_t val, v;

  if (pin >= port->ngpio)
    return;

  switch (drv) {
    case GPIO_DRV_WEAK:
      v = 0x0;
      break;

    case GPIO_DRV_WEAKER:
      v = 0x1;
      break;

    case GPIO_DRV_STRONGER:
      v = 0x2;
      break;

    case GPIO_DRV_STRONG:
      v = 0x3;
      break;

    default:
      v = 0x0;
      break;
  }

  addr = port->virt + GPIO_DRV0 + ((pin >> 4) << 2);
  val = read32(addr);
  val &= ~(v << ((pin & 0xf) << 1));
  val |= (v << ((pin & 0xf) << 1));
  write32(addr, val);
}

gpio_drv_t gpio_f1c100s_get_drv(const gpio_port_t* port, int32_t pin)
{
  uint32_t addr;
  uint32_t v = 0;

  if (pin >= port->ngpio)
    return GPIO_DRV_WEAK;

  addr = port->virt + GPIO_DRV0 + ((pin >> 4) << 2);
  v = (read32(addr) >> ((pin & 0xf) << 1)) & 0x3;

  switch (v) {
    case 0:
      return GPIO_DRV_WEAK;
    case 1:
      return GPIO_DRV_WEAKER;
    case 2:
      return GPIO_DRV_STRONGER;
    case 3:
      return GPIO_DRV_STRONG;
    default:
      break;
  }
  return GPIO_DRV_WEAK;
}

void gpio_f1c100s_set_rate(const gpio_port_t* port, int32_t pin, gpio_rate_t rate)
{
}

gpio_rate_t gpio_f1c100s_get_rate(const gpio_port_t* port, int32_t pin)
{
  return GPIO_RATE_SLOW;
}

void gpio_f1c100s_set_dir(const gpio_port_t* port, int32_t pin, gpio_direction_t dir)
{
  if (pin >= port->ngpio)
    return;

  switch (dir) {
    case GPIO_DIRECTION_INPUT:
      gpio_f1c100s_set_cfg(port, pin, 0);
      break;

    case GPIO_DIRECTION_OUTPUT:
      gpio_f1c100s_set_cfg(port, pin, 1);
      break;

    default:
      break;
  }
}

gpio_direction_t gpio_f1c100s_get_dir(const gpio_port_t* port, int32_t pin)
{
  if (pin >= port->ngpio)
    return GPIO_DIRECTION_INPUT;

  switch (gpio_f1c100s_get_cfg(port, pin)) {
    case 0:
      return GPIO_DIRECTION_INPUT;
    case 1:
      return GPIO_DIRECTION_OUTPUT;
    default:
      break;
  }
  return GPIO_DIRECTION_INPUT;
}

void gpio_f1c100s_set_value(const gpio_port_t* port, int32_t pin, int32_t value)
{
  uint32_t val;

  if (pin >= port->ngpio)
    return;

  val = read32(port->virt + GPIO_DAT);
  val &= ~(1 << pin);
  val |= (!!value) << pin;
  write32(port->virt + GPIO_DAT, val);
}

int32_t gpio_f1c100s_get_value(const gpio_port_t* port, int32_t pin)
{
  uint32_t val;

  if (pin >= port->ngpio)
    return 0;

  val = read32(port->virt + GPIO_DAT);
  return !!(val & (1 << pin));
}

int32_t gpio_f1c100s_to_irq(const gpio_port_t* port, int32_t pin)
{

  if ((pin >= port->ngpio) || (port->oirq < 0))
    return -1;
  return port->oirq + pin;
}

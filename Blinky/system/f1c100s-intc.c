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
#include "sizes.h"

#ifndef __IO
#define __IO volatile
#endif

typedef struct {
  __IO uint32_t INTC_VECTOR_REG;    // 0x00
  __IO uint32_t INTC_BASE_ADDR_REG; // 0x04
  uint32_t resv1[1];                // 0x08
  __IO uint32_t NMI_INT_CTRL_REG;   // 0x0c
  __IO uint32_t INTC_PEND_REG[2];   // 0x10
  uint32_t resv2[2];                // 0x18
  __IO uint32_t INTC_EN_REG[2];     // 0x20
  uint32_t resv3[2];                // 0x28
  __IO uint32_t INTC_MASK_REG[2];   // 0x30
  uint32_t resv4[2];                // 0x38
  __IO uint32_t INTC_RESP_REG[2];   // 0x40
  uint32_t resv5[2];                // 0x48
  __IO uint32_t INTC_FF_REG[2];     // 0x50
  uint32_t resv6[2];                // 0x58
  __IO uint32_t INTC_PRIO_REG[4];   // 0x60
} INTC_TypeDef;

#define INTC  ((INTC_TypeDef*)0x01C20400)

static IRQHandleTypeDef irq_table[64] __attribute__((used, zero_init, aligned(32)));

#ifndef __RTX

__irq void IRQ_Handler(void)
{
  uint8_t nIRQ = f1c100s_intc_get_nirq();

  // 必须由IRQ负责清除ForceIRQ标志，否则连续进入中断！
  INTC->INTC_FF_REG[nIRQ / 32] &= ~(1 << nIRQ);
  // Call the drivers ISR
  f1c100s_intc_dispatch(nIRQ);
  // Clear pending at the end of ISR.
  f1c100s_intc_clear_pend(nIRQ);
}

__irq void FIQ_Handler(void)
{
  // Call the drivers ISR
  f1c100s_intc_dispatch(0);
  // Clear pending at the end of ISR.
  f1c100s_intc_clear_pend(0);
}

#endif

/*
* 读取当前正处于激活状态的中断号
* @return: none
*/
uint8_t f1c100s_intc_get_nirq(void)
{
  return ((INTC->INTC_VECTOR_REG >> 2) & 0x3F);
}

/*
* 根据指定的nIRQ来执行对应的中断ISR
* @nIRQ: 中断号
* @return: none
*/
void f1c100s_intc_dispatch(uint8_t nIRQ)
{
  IRQHandleTypeDef handle = irq_table[nIRQ];
  if (handle)
    handle();
}

/*
* 设置指定外设的中断处理函数
* @nIRQ: 中断号
* @handle: 中断ISR
* @return: none
*/
void f1c100s_intc_set_isr(uint8_t nIRQ, void (*handle)(void))
{
  if (nIRQ < ARRAY_SIZE(irq_table)) {
    irq_table[nIRQ] = handle;
  }
}

/*
* 允许指定中断号产生中断请求
* @nIRQ: 中断号
* @return: none
*/
void f1c100s_intc_enable_irq(uint8_t nIRQ)
{
  CPU_SR_DECL;

  CPU_ENTER_CRITICAL();
  INTC->INTC_EN_REG[nIRQ / 32] |= (1 << (nIRQ % 32));
  CPU_EXIT_CRITICAL();
}

/*
* 禁止指定中断号产生中断请求
* @nIRQ: 中断号
* @return: none
*/
void f1c100s_intc_disable_irq(uint8_t nIRQ)
{
  CPU_SR_DECL;

  CPU_ENTER_CRITICAL();
  INTC->INTC_EN_REG[nIRQ / 32] &= ~(1 << (nIRQ % 32));
  CPU_EXIT_CRITICAL();
}

/*
* 屏蔽掉【即关闭】指定中断号产生中断请求
* @nIRQ: 中断号
* @return: none
*/
void f1c100s_intc_mask_irq(uint8_t nIRQ)
{
  CPU_SR_DECL;

  CPU_ENTER_CRITICAL();
  INTC->INTC_MASK_REG[nIRQ / 32] |= (1 << (nIRQ % 32));
  CPU_EXIT_CRITICAL();
}

/*
* 允许【即开启】指定中断号产生中断请求
* @nIRQ: 中断号
* @return: none
*/
void f1c100s_intc_unmask_irq(uint8_t nIRQ)
{
  CPU_SR_DECL;

  CPU_ENTER_CRITICAL();
  INTC->INTC_MASK_REG[nIRQ / 32] &= ~(1 << (nIRQ % 32));
  CPU_EXIT_CRITICAL();
}

/*
* 立即触发指定中断号产生中断请求
* @nIRQ: 中断号
* @return: none
*/
void f1c100s_intc_force_irq(uint8_t nIRQ)
{
  CPU_SR_DECL;

  // 这里只负责置位，必须由IRQ处理函数清除，否则会连续进入中断！
  CPU_ENTER_CRITICAL();
  INTC->INTC_FF_REG[nIRQ / 32] = (1 << (nIRQ % 32));
  CPU_EXIT_CRITICAL();
}

/*
* 清除INTC控制器的pending标志【实际上只对NMI起作用】
* @nIRQ: 中断号
* @return: none
*/
void f1c100s_intc_clear_pend(uint8_t nIRQ)
{
  INTC->INTC_PEND_REG[nIRQ / 32] = (1 << (nIRQ % 32));
}


/*
* intc模块初始化
* @return: none
*/
void f1c100s_intc_init(void)
{
  CPU_SR_DECL;

  CPU_ENTER_CRITICAL();
  INTC->INTC_EN_REG[0] = INTC->INTC_EN_REG[1] = 0;
  INTC->INTC_MASK_REG[0] = INTC->INTC_MASK_REG[1] = 0;
  INTC->INTC_FF_REG[0] = INTC->INTC_FF_REG[1] = 0;
  INTC->INTC_RESP_REG[0] = INTC->INTC_RESP_REG[1] = 0;
  INTC->INTC_PEND_REG[0] = INTC->INTC_PEND_REG[1] = ~0UL;
  INTC->INTC_BASE_ADDR_REG = 0;
  INTC->NMI_INT_CTRL_REG = 0;
  CPU_EXIT_CRITICAL();
  for (int i = 0; i < ARRAY_SIZE(irq_table); i++) {
    irq_table[i] = 0;
  }
}




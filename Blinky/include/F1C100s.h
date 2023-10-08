// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
#ifndef __LIB_F1C100S_H__
#define __LIB_F1C100S_H__

#include <stdint.h>

// 经过测试，F1C100s的[0x00000000-0x0000b5ff]空间有45.5kB的SRAM；
// 而[0x00010000-0x00019FFF]根本不存在40kB的SRAM A，真是datashit！

// spiflash启动时，BROM把用户的spl加载到0x00000000【SRAM空间】地址，
// 再把堆栈指针SP设置为0x00009FF4，然后跳转到spl代码。

// spiflash启动时，CPU处于SVC模式，IRQ和FIQ均关闭！

#ifdef __cplusplus
extern "C" {
#endif


/* types declared by <f1c100s-intc.c> */

typedef void (*IRQHandleTypeDef)(void);

uint8_t   f1c100s_intc_get_nirq(void);
void      f1c100s_intc_dispatch(uint8_t nIRQ);
void      f1c100s_intc_set_isr(uint8_t nIRQ, void (*handle)(void));
void      f1c100s_intc_enable_irq(uint8_t nIRQ);
void      f1c100s_intc_disable_irq(uint8_t nIRQ);
void      f1c100s_intc_unmask_irq(uint8_t nIRQ);
void      f1c100s_intc_mask_irq(uint8_t nIRQ);
void      f1c100s_intc_force_irq(uint8_t nIRQ);
void      f1c100s_intc_clear_pend(uint8_t nIRQ);
void      f1c100s_intc_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __LIB_F1C100S_H__ */

// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
#ifndef __LIB_F1C100S_H__
#define __LIB_F1C100S_H__

#include <stdint.h>

// 经过测试，F1C100s的[0x00000000-0x0000b5ff]空间有45.5kB的SRAM；
// 而[0x00010000-0x00019FFF]根本不存在40kB的SRAM A，真是datashit！

// spiflash启动时，BROM把用户的spl加载到0x00000000【SRAM空间】地址，
// 再把堆栈指针SP设置为0x00009FF4，然后跳转到spl代码。

// spiflash启动时，CPU处于SVC模式，IRQ和FIQ均关闭！

// 若是fel加载运行，spl的地址仍然是0x00000000，但SP是0x0000B418

#ifdef __cplusplus
extern "C" {
#endif

#define CLK_CPU     (720*1000000UL)
#define CLK_PERI    (600*1000000UL)
#define CLK_AHB     (200*1000000UL)
#define CLK_APB     (100*1000000UL)

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

#define I2C_M_TEN           0x0010    /* this is a ten bit chip address */
#define I2C_M_RD            0x0001    /* read data, from slave to master */
#define I2C_M_NOSTART       0x4000    /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_REV_DIR_ADDR  0x2000    /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK    0x1000    /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NO_RD_ACK     0x0800    /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_RECV_LEN      0x0400    /* length will be first received byte */

typedef struct {
  uint16_t addr;      /* slave address            */
  uint16_t flags;     /* message flags I2C_M_XX   */
  uint16_t len;       /* msg length               */
  uint8_t* buf;       /* pointer to msg data      */
} I2CMsg_TypeDef;

#define TWI0_MID    (0)
#define TWI1_MID    (1)
#define TWI2_MID    (2)

/* types declared by <f1c100s-iic.c> */
void      f1c100s_twi_init(uint8_t mid, uint32_t freq);
int       f1c100s_twi_xfer(uint8_t mid, I2CMsg_TypeDef msgs[], int num);


#define UART0_MID   (0)
#define UART1_MID   (1)
#define UART2_MID   (2)

/* types declared by <f1c100s-uart.c> */
bool_t   f1c100s_uart_init(uint8_t mid);
int      f1c100s_uart_readFifo(uint8_t mid, uint8_t buf[], unsigned int max);
void     f1c100s_uart_clearfifo(uint8_t mid);
void     f1c100s_uart_setBaudrate(uint8_t mid, uint32_t baud);
uint32_t f1c100s_uart_getBaudrate(uint8_t mid);
void     f1c100s_uart_setRxCallback(uint8_t mid, void (*pf)(void));
void     f1c100s_uart_setTxCallback(uint8_t mid, void (*pf)(void));
void     f1c100s_uart_asyncTx(uint8_t mid, const void* ptr, unsigned int len);
bool_t   f1c100s_uart_isTxEnd(uint8_t mid);

#ifdef __cplusplus
}
#endif

#endif /* __LIB_F1C100S_H__ */

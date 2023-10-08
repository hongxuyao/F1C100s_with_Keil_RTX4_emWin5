// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
#ifndef __FDEBUG_DEF_H__
#define __FDEBUG_DEF_H__
////////////////////////////////////////////////////////////////////////////////
#include "ftypes.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C"  {
#endif

#if defined(FDEBUG_EN) && (FDEBUG_EN > 0)
extern void fdbg_init(void);
extern void fdbg_flush(bool_t bClose);
extern void fdbg_putc(int c);
extern void fdbg_puts(char const s[]);
extern void fdbg_rxchar(char c);
extern void fdbg_output(void const* bin, unsigned num);
#define DBG_INIT()       fdbg_init()
#define DBG_CLOSE()      fdbg_flush(1)
#define DBG_PUTC(c)      fdbg_putc(c)
#define DBG_PUTS(s)      fdbg_puts(s)
#define DBG_OUTPUT(s,n)  fdbg_output(s,n)
#define DBG_PRINT(...)   printf(__VA_ARGS__)
#else
#define DBG_INIT()
#define DBG_CLOSE()
#define DBG_PUTC(c)
#define DBG_PUTS(s)
#define DBG_OUTPUT(s,n)
#define DBG_PRINT(...)
#endif

#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////
#endif /* __FDEBUG_DEF_H__ */


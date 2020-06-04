// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
#ifndef __FRAME_TYPES__H__
#define __FRAME_TYPES__H__
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>

#ifdef __cplusplus
extern "C"  {
#endif

typedef signed char         s8_t;
typedef unsigned char       u8_t;
typedef signed short        s16_t;
typedef unsigned short      u16_t;
typedef signed int          s32_t;
typedef unsigned int        u32_t;
typedef signed long long    s64_t;
typedef unsigned long long  u64_t;
typedef float               f32_t;
typedef double              f64_t;
typedef unsigned char       bool_t;
typedef signed long         sbase_t;
typedef unsigned long       ubase_t;
typedef void*               pvoid_t;
typedef void const*         pcvoid_t;

#if defined (__CC_ARM)

#if defined (__BIG_ENDIAN)
#define IS_BIG_ENDIAN       (1)
#else
#define IS_BIG_ENDIAN       (0)
#endif

#elif defined (__ICCARM__)

#if __LITTLE_ENDIAN__ > 0
#define IS_BIG_ENDIAN       (0)
#else
#define IS_BIG_ENDIAN       (1)
#endif

#else

#error IS_BIG_ENDIAN未定义！

#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((pvoid_t)0)
#endif
#endif

#ifndef TRUE
#define TRUE (!0)
#endif

#ifndef FALSE
#define FALSE (!1)
#endif

#define true  TRUE
#define false FALSE

#ifndef COUNTOF
#define COUNTOF(ar) (sizeof(ar)/sizeof(ar[0]))
#endif

#ifndef OFFSET
#define OFFSET(t,m) ((u32_t)&((t*)0)->m)
#endif

#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////
#endif /* __FRAME_TYPES__H__ */


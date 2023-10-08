// 本文件由洪旭耀设计，而使用权分发权自由属于你！
// 可以通过QQ联系本人：26750452【24小时在线守候】
#ifndef __TARGET_H__
#define __TARGET_H__
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include "ftypes.h"
#include "mmu.h"
#include "F1C100s.h"

#define MEM_PI_SRAM     __attribute__((section("SRAM")))

#define MEM_PI_SUMMARY  __attribute__((section("SUMMARY")))

#define MEM_PI_NOINIT   __attribute__((section("NOINIT"),zero_init))

#define MEM_PI_CPUONLY  __attribute__((section("CPUONLY"),zero_init))

#define MEM_PI_HARDWARE __attribute__((section("HARDWARE"),zero_init,aligned(32)))

#define MEM_PI_NCNB     __attribute__((section("NCNB"),zero_init,aligned(32)))

#define MEM_PI_STACK    __attribute__((section("STACK"),zero_init,aligned(8)))

#define CACHE_ALIGNED   __attribute__((aligned(32)))

#ifndef INLINE
#define INLINE        __attribute__((always_inline))
#endif

#ifndef NOINLINE
#define NOINLINE      __attribute__((noinline))
#endif

#ifndef NOINLINE_FUNC
#define NOINLINE_FUNC __attribute__((noinline))
#endif

#ifndef ALIGN
#define ALIGN(n)      __attribute__((aligned(n)))
#endif

#define CPU_SR_DECL   ubase_t cpu_sr

#ifdef __thumb__  /* 为了让内联汇编在thumb编译模式下能编译通过(但是会失去内联性能) */
#define __SWITCH_TO_ARM__
#pragma arm
#endif

#if 0
#define CPU_ENTER_CRITICAL()  do{cpu_sr = util_enter_critical();}while(0)
#else
#define CPU_ENTER_CRITICAL()  do{cpu_sr = __fast_enter_critical();}while(0)
static inline ubase_t __fast_enter_critical(void)
{
  ubase_t cpu_sr, tmp_sr;
  __asm volatile {
    MRS cpu_sr, CPSR
    ORR tmp_sr, cpu_sr, #0xC0
    MSR CPSR_c, tmp_sr
  }
  return cpu_sr;
}
#endif

#if 0
#define CPU_EXIT_CRITICAL()   do{util_exit_critical(cpu_sr);}while(0)
#else
#define CPU_EXIT_CRITICAL()   do{__fast_exit_critical(cpu_sr);}while(0)
static inline void __fast_exit_critical(ubase_t cpu_sr)
{
  __asm volatile {
    MSR CPSR_c, cpu_sr
  }
}
#endif

static inline unsigned CPU_CLZ16(uint16_t val)
{
  __asm volatile {
    CLZ val, val
  }
  return (val - 16);
}

static inline uint8_t __swap_byte(uint8_t newval, uint8_t volatile* pmem)
{
  uint8_t oldval;
  __asm volatile {
    SWPB  oldval, newval, [pmem]
  };
  return oldval;
}

static inline uint32_t UTL_REV32(uint32_t val)
{
  uint32_t tmpval;
  __asm volatile {
    EOR tmpval, val, val, ROR #16
    MOV tmpval, tmpval, LSR #8
    BIC tmpval, tmpval, #0xFF00
    EOR val, tmpval, val, ROR #8
  }
  return val;
}

static inline uint16_t UTL_REV16(uint16_t val)
{
  uint32_t tmpval;
  __asm volatile {
    LSR tmpval, val, #8
    ORR val, tmpval, val, LSL #8
    BIC val, val, #0xFF0000
  }
  return val;
}

#ifdef __SWITCH_TO_ARM__  /* 内联汇编定义结束，恢复thumb编译模式 */
#undef __SWITCH_TO_ARM__
#pragma thumb
#endif

#ifndef COUNTOF
#define COUNTOF(ar) (sizeof(ar)/sizeof(ar[0]))
#endif

/* 定义在<ubox_a.s>文件中的实用函数 */
void    util_halt(void);
void    util_fastloop(ubase_t n);
ubase_t util_getCPSR(void);
ubase_t util_enter_critical(void);
void    util_exit_critical(ubase_t sr);
void    util_enable_interrupt(void);
void    util_disable_interrupt(void);

/* 定义在<target_DEV.c>文件中的函数 */
void    target_wdt_setup(void);
void    target_wdt_feed(void);
void    target_reset(void);

////////////////////////////////////////////////////////////////////////////////
#endif /* __TARGET_H__ */


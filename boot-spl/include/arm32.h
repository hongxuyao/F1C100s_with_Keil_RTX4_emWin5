#ifndef __ARM32_H__
#define __ARM32_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

static inline uint32_t arm32_read_p15_c1(void)
{
  uint32_t value;

  __asm volatile {
    mrc p15, 0, value, c1, c0, 0
  }
  return value;
}

static inline void arm32_write_p15_c1(uint32_t value)
{
  __asm volatile {
    mcr p15, 0, value, c1, c0, 0
  }
  arm32_read_p15_c1();
}

static inline void arm32_interrupt_enable(void)
{
  uint32_t tmp;

  __asm volatile {
    mrs tmp, cpsr
    bic tmp, tmp, #(1 << 7)
    msr cpsr_cxsf, tmp
  }
}

static inline void arm32_interrupt_disable(void)
{
  uint32_t tmp;

  __asm volatile {
    mrs tmp, cpsr
    orr tmp, tmp, #(1 << 7)
    msr cpsr_cxsf, tmp
  }
}

static inline void arm32_mmu_enable(void)
{
  uint32_t value = arm32_read_p15_c1();
  arm32_write_p15_c1(value | (1 << 0));
}

static inline void arm32_mmu_disable(void)
{
  uint32_t value = arm32_read_p15_c1();
  arm32_write_p15_c1(value & ~(1 << 0));
}

static inline void arm32_dcache_enable(void)
{
  uint32_t value = arm32_read_p15_c1();
  arm32_write_p15_c1(value | (1 << 2));
}

static inline void arm32_dcache_disable(void)
{
  uint32_t value = arm32_read_p15_c1();
  arm32_write_p15_c1(value & ~(1 << 2));
}

static inline void arm32_icache_enable(void)
{
  uint32_t value = arm32_read_p15_c1();
  arm32_write_p15_c1(value | (1 << 12));
}

static inline void arm32_icache_disable(void)
{
  uint32_t value = arm32_read_p15_c1();
  arm32_write_p15_c1(value & ~(1 << 12));
}

static inline uint32_t arm32_smp_processor_id(void)
{
  uint32_t tmp;

  __asm volatile {
    mrc p15, 0, tmp, c0, c0, 5
    and tmp, tmp, #0x3
  }
  return tmp;
}

static inline void arm32_ttb_set(uint32_t base)
{
  __asm volatile {
    mcr p15, 0, base, c2, c0, 0
  }
}

static inline void arm32_domain_set(uint32_t domain)
{
  __asm volatile {
    mcr p15, 0, domain, c3, c0, 0
  }
}

static inline void arm32_tlb_invalidate(void)
{
  uint32_t cpu_reg0;
  __asm volatile {
    mov cpu_reg0, #0
    mcr p15, 0, cpu_reg0, c7, c10, 4
    mcr p15, 0, cpu_reg0, c8, c6, 0
    mcr p15, 0, cpu_reg0, c8, c5, 0
  }
}

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_H__ */

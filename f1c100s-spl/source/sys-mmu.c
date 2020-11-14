#include <stdint.h>
#include "io.h"

#define SIZ_1G    (1UL<<30)
#define SIZ_1M    (1UL<<20)
#define SIZ_1K    (1UL<<10)

static __inline void arm32_ttb_set(uint32_t base)
{
  __asm volatile {
    mcr p15, 0, base, c2, c0, 0
  }
}

static __inline void arm32_tlb_invalidate(void)
{
  uint32_t cpu_reg0;
  __asm volatile {
    mov cpu_reg0, #0
    mcr p15, 0, cpu_reg0, c7, c10, 4
    mcr p15, 0, cpu_reg0, c8, c6, 0
    mcr p15, 0, cpu_reg0, c8, c5, 0
  }
}

static __inline void arm32_domain_set(uint32_t domain)
{
  __asm volatile {
    mcr p15, 0, domain, c3, c0, 0
  }
}

static __inline uint32_t arm32_read_p15_c1(void)
{
  uint32_t value;

  __asm volatile {
    mrc p15, 0, value, c1, c0, 0
  }
  return value;
}

static __inline void arm32_write_p15_c1(uint32_t value)
{
  __asm volatile {
    mcr p15, 0, value, c1, c0, 0
  }
  arm32_read_p15_c1();
}

static __inline void arm32_mmu_enable(void)
{
  uint32_t value = arm32_read_p15_c1();
  arm32_write_p15_c1(value | (1 << 0));
}

static __inline void arm32_icache_enable(void)
{
  uint32_t value = arm32_read_p15_c1();
  arm32_write_p15_c1(value | (1 << 12));
}

static __inline void arm32_dcache_enable(void)
{
  uint32_t value = arm32_read_p15_c1();
  arm32_write_p15_c1(value | (1 << 2));
}

static void map_l1_section(uint32_t* ttb, uint32_t virt, uint32_t phys, uint32_t size, int type)
{
  uint32_t i;

  virt >>= 20;
  phys >>= 20;
  size >>= 20;
  type &= 0x3;

  for (i = size; i > 0; i--, virt++, phys++) {
    // 填写接描述符，每个节描述符影响到1MB虚拟空间
    ttb[virt] = ((phys << 20) | /* 物理地址的高12位 */
                 (0x3 << 10) |  /* AP位，指定访问许可 */
                 (0x0 << 5) |   /* 指定16个可能的域之一 */
                 (type << 2) |  /* C和B位，控制cache和buffer */
                 (0x12));       /* 表示这是节描述符 */
  }
}

void sys_mmu_init(void)
{
  uint32_t* ttb = (uint32_t*)(0x80004000);    // TTB放在0x80004000处
  uint32_t dram_siz = *(uint32_t volatile*)0x5c;

  // SRAM的0x5c地址，被DRAM模块设置为DDR容量。
  if ((dram_siz >> 24) == (uint8_t)'X') {
    dram_siz &= 0x0FFF;
  } else {
    dram_siz = 32;
  }
  dram_siz = dram_siz * SIZ_1M; /* 把容量数值转换为字节数 */

  map_l1_section(ttb, 0x00000000, 0x00000000, 2 * SIZ_1G, 0);
  map_l1_section(ttb, 0x80000000, 0x80000000, 2 * SIZ_1G, 0);
  map_l1_section(ttb, 0x00000000, 0x00000000, 1 * SIZ_1M, 2); // 向量表在此，CNB
  map_l1_section(ttb, 0x80000000, 0x80000000, dram_siz, 3);   // SDRAM空间，CB

  arm32_ttb_set((uint32_t)(ttb));
  arm32_tlb_invalidate();
  arm32_domain_set(0x3);
  arm32_mmu_enable();
  arm32_icache_enable();
  arm32_dcache_enable();
}


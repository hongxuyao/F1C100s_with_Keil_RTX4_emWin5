#include <stdint.h>
#include "reg-ccu.h"
#include "io.h"

/*
  时钟配置如下：
  PLL_PERIPH = 600Mhz
  CPU_CLK = 720Mhz
  AHB_CLK = 200Mhz
  APB_CLK = 100Mhz
*/

extern void loop_delay(uint32_t nloops);

/*
* 等待PLL输出稳定
* @base: 某PLL控制寄存器地址
* @return: none
*/
static void wait_pll_stable(uint32_t base)
{
  uint32_t rval;
  for (uint32_t time = 0xffff; time; time--) {
    rval = read32(base);
    if (rval & (1 << 28))
      break;
  }
}

/*
* 配置PLL_CPU
* @clk: CPU时钟频率【hz】
* @return: none
*/
static void clock_set_pll_cpu(uint32_t clk)
{
  uint32_t n, k, m, p;
  uint32_t rval = 0;
  uint32_t div = 0;

  /*
  PLL_CPU output = (24MHz*N*K)/(M*P).
  N = [1,...,32]
  K = [1,2,3,4]
  M = [1,2,3,4]
  P = [1,2,4]
  */

  if (clk > 720000000)
    clk = 720000000;

  if ((clk % 24000000) == 0) {
    div = clk / 24000000;
    n = div - 1;
    k = 0;
    m = 0;
    p = 0;
  } else if ((clk % 12000000) == 0) {
    div = clk / 12000000;
    if ((div % 3) == 0)
      k = 2;
    else if ((div % 4) == 0)
      k = 3;
    else
      k = 1;
    n = (div / (k + 1)) - 1;
    m = 1;
    p = 0;
  } else {
    div = clk / 24000000;
    n = div - 1;
    k = 0;
    m = 0;
    p = 0;
  }

  // The PLL output = (24MHz*N*K)/(M*P).
  rval = read32(F1C100S_CCU_BASE + CCU_PLL_CPU_CTRL);
  rval &= ~((0x3 << 16) | (0x1f << 8) | (0x3 << 4) | (0x3 << 0));
  rval |= ((1U << 31) | (p << 16) | (n << 8) | (k << 4) | m);
  write32(F1C100S_CCU_BASE + CCU_PLL_CPU_CTRL, rval);
  wait_pll_stable(F1C100S_CCU_BASE + CCU_PLL_CPU_CTRL);
}

#define HCLKC_DIV         (0) // HCLKC=CPUCLK/1
#define AHB_CLK_SRC_SEL   (3) // AHB source is (PLL_PERIPH/AHB_PRE_DIV)
#define APB_CLK_RATIO     (1) // APB_CLK=AHB_CLK/2=100Mhz
#define AHB_PRE_DIV       (2) // AHB_source=600/3=200Mhz
#define AHB_CLK_DIV_RATIO (0) // AHB_CLK=AHB_source/1=200Mhz
void sys_clock_init(void)
{
  uint32_t val;

  // 【1】设置PLL和CPU-PLL的锁定延迟时间
  write32(F1C100S_CCU_BASE + CCU_PLL_STABLE_TIME0, 0x1ff);
  write32(F1C100S_CCU_BASE + CCU_PLL_STABLE_TIME1, 0x1ff);

  // 【2】先把24Mhz晶振作为CPU时钟源
  val = read32(F1C100S_CCU_BASE + CCU_CPU_CFG);
  val &= ~(0x3 << 16);
  val |= (0x1 << 16);
  write32(F1C100S_CCU_BASE + CCU_CPU_CFG, val);
  loop_delay(100);

  // 【3】设置PLL_VIDEO(N=66,M=8), In the integer mode, the PLL output = (24MHz*N)/M.
  write32(F1C100S_CCU_BASE + CCU_PLL_VIDEO_CTRL, ((1U << 31) | (0 << 30) | (1 << 24) | (65 << 8) | (7)));
  loop_delay(100);

  // 【4】设置PLL_PERIPH(N=25,M=1) to 600Mhz, The PLL Output = 24MHz*N*K.
  write32(F1C100S_CCU_BASE + CCU_PLL_PERIPH_CTRL, ((1U << 31) | (1 << 18) | (0 << 16) | (24 << 8) | (0 << 4) | (0)));
  loop_delay(100);

  // 【5】设置AHB/APB/HCLKC的工作频率
  write32(F1C100S_CCU_BASE + CCU_AHB_APB_CFG,
          ((HCLKC_DIV << 16) |
           (AHB_CLK_SRC_SEL << 12) |
           (APB_CLK_RATIO << 8) |
           (AHB_PRE_DIV << 6) |
           (AHB_CLK_DIV_RATIO << 4)));
  loop_delay(100);

  // 【6】开启DE的front-end和back-end的DCLK？
  val = read32(F1C100S_CCU_BASE + CCU_DRAM_CLK_GATE);
  val |= (0x1 << 26) | (0x1 << 24);
  write32(F1C100S_CCU_BASE + CCU_DRAM_CLK_GATE, val);
  loop_delay(100);

  // 【7】配置PLL_CPU的值
  clock_set_pll_cpu(720000000);

  // 【8】把CPU时钟源设置为PLL_CPU
  val = read32(F1C100S_CCU_BASE + CCU_CPU_CFG);
  val &= ~(0x3 << 16);
  val |= (0x2 << 16);
  write32(F1C100S_CCU_BASE + CCU_CPU_CFG, val);
  loop_delay(100);
}


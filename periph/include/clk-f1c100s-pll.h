#ifndef __CLK_F1C100S_PLL_H__
#define __CLK_F1C100S_PLL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct clk_pll_st {
  uint32_t virt;
  void* parent;
  int32_t channel;
  uint32_t prate;
} clk_pll_t;

void clk_f1c100s_pll_set_parent(clk_pll_t* clk_pll, const char* pname);

const clk_pll_t* clk_f1c100s_pll_get_parent(clk_pll_t* clk_pll);

void clk_f1c100s_pll_set_enable(clk_pll_t* clk_pll, bool enable);
bool clk_f1c100s_pll_get_enable(clk_pll_t* clk_pll);

void clk_f1c100s_pll_set_rate(clk_pll_t* clk_pll, uint64_t prate, uint64_t rate);

uint64_t clk_f1c100s_pll_get_rate(clk_pll_t* clk_pll, uint64_t prate);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_F1C100S_PLL_H__ */


#ifndef __RESET_F1C100S_H__
#define __RESET_F1C100S_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct reset_f1c100s_st {
  uint32_t virt;
  int32_t base;
  int32_t nreset;
} reset_f1c100s_t;

void reset_f1c100s_assert(reset_f1c100s_t* reset, int32_t offset);
void reset_f1c100s_deassert(reset_f1c100s_t* reset, int32_t offset);

#ifdef __cplusplus
}
#endif

#endif /* __RESET_F1C100S_H__ */

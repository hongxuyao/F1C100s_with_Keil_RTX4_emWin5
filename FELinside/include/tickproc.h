// 本文件由洪旭耀设计，而使用权分发权自由属于你！
// 可以通过QQ联系本人：26750452【24小时在线守候】
#ifndef __TICK_PROC_H__
#define __TICK_PROC_H__
////////////////////////////////////////////////////////////////////////////////
#include "ftypes.h"

#ifdef __cplusplus
extern "C"  {
#endif

typedef struct __TickNodeType {
  struct __TickNodeType* next;
  void (*callback)(void);
} TickNodeType;

extern void TICK_Exec(void);
extern void TICK_Pause(void);
extern void TICK_Resume(void);
extern void TICK_Add(TickNodeType* node);
extern void TICK_Del(TickNodeType* node);

#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////
#endif /* __TICK_PROC_H__ */


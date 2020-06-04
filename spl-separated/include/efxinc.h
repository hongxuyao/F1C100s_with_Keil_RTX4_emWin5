// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
#ifndef __EFX_INC_H__
#define __EFX_INC_H__

#include "ftypes.h"
#include "fdebug.h"
#include "target.h"
#include <stdlib.h>
#include <string.h>
#ifdef __RTX
#include "RTL.h"
#endif
#include "tickproc.h"

#ifdef __cplusplus
extern "C"  {
#endif

#define TWAIT_FOREVER       (0xFFFF)  // 不超时

#define TSK_PRIO_RESERVED   (255) // 保留优先级
#define TSK_PRIO_HILIMIT    (254) // 优先级上限
#define TSK_PRIO_HIGHEST    (50)  // 最高优先级
#define TSK_PRIO_HIGHER     (40)  // 偏高优先级
#define TSK_PRIO_NORMAL     (30)  // 普通优先级
#define TSK_PRIO_LOWER      (20)  // 偏低优先级
#define TSK_PRIO_LOWEST     (10)  // 最低优先级
#define TSK_PRIO_LOLIMIT    (1)   // 优先级下限
#define TSK_PRIO_IDLE       (0)   // 空闲优先级

extern volatile U32 os_time;      // 从rt_Time.o模块导出
#define sys_tick os_time          // 必须是CPU数据宽度

static inline void timer_start(U32* pTimer)
{
  *pTimer = sys_tick;
}

static inline int timer_expired(U32* pTimer, U32 dly)
{
  U32 elapsed = sys_tick - *pTimer;
  return (elapsed >= dly);
}

#define sys_delay(ms) os_dly_wait(ms)

// 当前线程主动让出CPU给下一个(同优先级)线程
#define sys_yield() os_dly_wait(1)

// 挂起当前调用线程，可以用OSTaskResume()函数使之继续。
static inline void sys_suspend(void)
{
  os_dly_wait(TWAIT_FOREVER);
}

/* 定义在<RTX_Conf_xxx.c>文件中 */
void   UFX_SetupTaskTimer(void);
bool_t UFX_AddTaskTimer(U16 dly, void (*pfn)(void*), void* parg);
/* 定义在<drv-loader.c>文件中 */
bool_t UFX_LoadDrivers(void);
/* 用户代码初始化 */
void   UserEntryInit(void);
/* 用户代码循环 */
void   UserEntryLoop(void);



#ifdef __cplusplus
}
#endif

#endif /* __EFX_INC_H__ */




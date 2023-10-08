// 本文件由洪旭耀设计，而使用权分发权自由属于你！
// 可以通过QQ联系本人：26750452【24小时在线守候】
#include "target.h"

////////////////////////////////////////////////////////////////////////////////
// 提供复位整个MCU的功能，包括CPU内核及外围设备。通常利用WDT。
////////////////////////////////////////////////////////////////////////////////
static volatile bool_t bReset = 0;

void target_reset(void)
{
  CPU_SR_DECL;

  bReset = true;
  CPU_ENTER_CRITICAL();
  CPU_EXIT_CRITICAL();
  util_halt();
}

void target_wdt_setup(void)
{
  CPU_SR_DECL;

  CPU_ENTER_CRITICAL();
  CPU_EXIT_CRITICAL();
}

void target_wdt_feed(void)
{
  CPU_SR_DECL;

  if (!bReset) {
    CPU_ENTER_CRITICAL();
    CPU_EXIT_CRITICAL();
  }
}


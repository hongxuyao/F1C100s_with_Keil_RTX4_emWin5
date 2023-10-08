// 本文件由洪旭耀设计，而使用权分发权自由属于你！
// 可以通过QQ联系本人：26750452【24小时在线守候】
#include "usrinc.h"

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: UserEntryInit
//| 功能描述 |: 应用程序初始化
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
void UserEntryInit(void)
{
  target_wdt_feed();
  printf("DDR size: %uMB\n", (*(uint32_t*)0x5c) & 0xFFFFFF);
  if (!APP_CreateGuiService()) {
    sys_suspend();
  }
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: UserEntryLoop
//| 功能描述 |: 应用程序主循环
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
void UserEntryLoop(void)
{
  for (;;) {
    target_wdt_feed();
    // 循环间隔
    sys_delay(500);
  }
}


////////////////////////////////////////////////////////////////////////////////


// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
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

  f1c100s_twi_init(TWI0_MID, 400 * 1000);

  f1c100s_uart_init(UART0_MID);
  f1c100s_uart_setBaudrate(UART0_MID, 115200);
  f1c100s_uart_clearfifo(UART0_MID);

  if (!APP_CreateGuiService()) {
    sys_suspend();
  }

  if (!APP_CreateTouchService()) {
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
#if 0
    // 串口驱动测试
    if (f1c100s_uart_isTxEnd(UART0_MID)) {
      rCnt = f1c100s_uart_readFifo(UART0_MID, rbuffer, sizeof(rbuffer));
      if (rCnt > 0) {
        f1c100s_uart_asyncTx(UART0_MID, rbuffer, rCnt);
      }
    }
#endif
  }
}


////////////////////////////////////////////////////////////////////////////////


// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
#include "usrinc.h"
#include "LCDConf.h"
#include "GUI.h"
#include "DIALOG.h"
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
static U64 __StackGUI[16384 / 8] MEM_PI_STACK;
static __task void __ThreadGUI(void);
static OS_TID _gui_tid = 0;

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: APP_CreateGuiService
//| 功能描述 |: 创建GUI线程并启动GUI服务
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
bool_t APP_CreateGuiService(void)
{
  _gui_tid = os_tsk_create_user(
                 __ThreadGUI,
                 TSK_PRIO_LOLIMIT,
                 __StackGUI,
                 sizeof(__StackGUI));

  if (_gui_tid == 0) {
    DBG_PUTS("GUI failed.\n");
    return false;
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __ThreadGUI
//| 功能描述 |: GUI线程
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static __task void __ThreadGUI(void)
{
  puts("RTX+emWin");
  MainTask();
  os_tsk_delete_self();
}

////////////////////////////////////////////////////////////////////////////////


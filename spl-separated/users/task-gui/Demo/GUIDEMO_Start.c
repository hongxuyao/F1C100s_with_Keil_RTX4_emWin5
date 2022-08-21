/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2014  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.24 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIDEMO_Start.c
Purpose     : GUIDEMO initialization
----------------------------------------------------------------------
*/

#include "GUIDEMO.h"

#include <stdint.h>
extern uint8_t const doTSCali;
extern WM_HWIN DLG_CreateTSCali(void);
/*********************************************************************
*
*       MainTask
*/
void MainTask(void)
{
#if GUI_WINSUPPORT
  WM_SetCreateFlags(WM_CF_MEMDEV);
#endif
  GUI_Init();
  if (doTSCali) {
    GUI_ExecCreatedDialog(DLG_CreateTSCali());
  }
#if GUI_WINSUPPORT
  //  WM_MULTIBUF_Enable(1);
#endif
  GUIDEMO_Main();
}

/*************************** End of file ****************************/


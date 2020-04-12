// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
#include "efxinc.h"
#include "RTL.h"
#include "GUI.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Define the available number of bytes available for the GUI
//
#ifndef GUI_NUMBYTES
#define GUI_NUMBYTES  (4*1024*1024UL)
#endif
//
// Define the average block size
//
#define GUI_BLOCKSIZE 0x80

/*********************************************************************
*
*       Global data
*/

/*********************************************************************
*
*      Timing:
*                 GUI_X_GetTime()
*                 GUI_X_Delay(int)

  Some timing dependent routines require a GetTime
  and delay function. Default time unit (tick), normally is
  1 ms.
*/

GUI_TIMER_TIME GUI_X_GetTime(void)
{
  return sys_tick;
}

void GUI_X_Delay(int ms)
{
  while (ms > 0xFFFE) {
    ms -= 0xFFFE;
    os_dly_wait(0xFFFE);
  }
  os_dly_wait(ms);
}

/*********************************************************************
*
*       GUI_X_Init()
*
* Note:
*     GUI_X_Init() is called from GUI_Init is a possibility to init
*     some hardware which needs to be up and running before the GUI.
*     If not required, leave this routine blank.
*/

void GUI_X_Init(void)
{

}

/*********************************************************************
*
*       GUI_X_ExecIdle
*
* Note:
*  Called if WM is in idle state
*/

void GUI_X_ExecIdle(void)
{
  os_dly_wait(1);
}

/*********************************************************************
*
*      Logging: OS dependent

Note:
  Logging is used in higher debug levels only. The typical target
  build does not use logging and does therefor not require any of
  the logging routines below. For a release build without logging
  the routines below may be eliminated to save some space.
  (If the linker is not function aware and eliminates unreferenced
  functions automatically)

*/

void GUI_X_Log(const char* s) { DBG_PUTS(s); }
void GUI_X_Warn(const char* s) { DBG_PUTS(s); }
void GUI_X_ErrorOut(const char* s) { DBG_PUTS(s); }

/*********************************************************************
*
*      Multitasking:
*
*                 GUI_X_InitOS()
*                 GUI_X_GetTaskId()
*                 GUI_X_Lock()
*                 GUI_X_Unlock()
*
* Note:
*   The following routines are required only if emWin is used in a
*   true multi task environment, which means you have more than one
*   thread using the emWin API.
*   In this case the
*                       #define GUI_OS 1
*  needs to be in GUIConf.h
*/

void GUI_X_InitOS(void)    {  }
void GUI_X_Unlock(void)    {  }
void GUI_X_Lock(void)      {  }
U32  GUI_X_GetTaskId(void) { return 0; }

/*********************************************************************
*
*       GUI_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   available memory for the GUI.
*/
void GUI_X_Config(void)
{
  //
  // 64 bit aligned memory area
  //
  static U64 aMemory[GUI_NUMBYTES / 8] MEM_PI_CPUONLY;
  //
  // Assign memory to emWin
  //
  GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
  GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE);
  GUI_SetOnErrorFunc(GUI_X_ErrorOut);
  GUITASK_SetMaxTask(1);
  //
  // Set default font
  //
  GUI_SetDefaultFont(GUI_DEFAULT_FONT);
}


/*************************** End of file ****************************/

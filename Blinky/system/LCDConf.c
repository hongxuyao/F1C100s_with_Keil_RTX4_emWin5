// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
#include "GUI.h"
#include "GUIDRV_Lin.h"
#include "framebuffer.h"
#include "f1c100s/reg-tcon.h"
#include "f1c100s-irq.h"
#include "target.h"
#include "mmu.h"
#include "io.h"

/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************
*/
//
// Physical display size
//
#define XSIZE_PHYS  (480)
#define YSIZE_PHYS  (272)

//
// Color conversion
//
#define COLOR_CONVERSION GUICC_8888

//
// Display driver
//
#define DISPLAY_DRIVER GUIDRV_LIN_32

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   XSIZE_PHYS
#error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
#error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
#error Color conversion not defined!
#endif
#ifndef   DISPLAY_DRIVER
#error No display driver defined!
#endif

static framebuffer_t fb_f1c100s;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
static void lcd_frame_irq(void)
{
  write32(F1C100S_TCON_BASE + 0x04, (1U << 31));
  //MMU_TestCleanDCache();
}

/*********************************************************************
*
*       _InitController
*
* Purpose:
*   Should initialize the display controller
*/
static void _InitController(void)
{
  f1c100s_intc_set_isr(F1C100S_IRQ_LCD, lcd_frame_irq);
  fb_f1c100s_init(&fb_f1c100s);
  fb_f1c100s_present(&fb_f1c100s, 0);
  fb_f1c100s_setbl(&fb_f1c100s, 100);
  write32(F1C100S_TCON_BASE + 0x04, (1U << 31));
  f1c100s_intc_enable_irq(F1C100S_IRQ_LCD);
}

/*********************************************************************
*
*       _SetVRAMAddr
*
* Purpose:
*   Should set the frame buffer base address
*/
static void _SetVRAMAddr(void* pVRAM)
{

}

/*********************************************************************
*
*       _SetOrg
*
* Purpose:
*   Should set the origin of the display typically by modifying the
*   frame buffer base address register
*/
static void _SetOrg(int xPos, int yPos)
{
  /* TBD by customer */
}

/*********************************************************************
*
*       _SetLUTEntry
*
* Purpose:
*   Should set the desired LUT entry
*/
static void _SetLUTEntry(LCD_COLOR Color, U8 Pos)
{
  /* TBD by customer */
}
/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/
void LCD_X_Config(void)
{
  //
  // Set display driver and color conversion for 1st layer
  //
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
  //
  // Display driver configuration
  //
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx(0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS, XSIZE_PHYS);
  } else {
    LCD_SetSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
  }
  LCD_SetVRAMAddrEx(0, (void*)fb_f1c100s_get_vram1());
  //
  // Set user palette data (only required if no fixed palette is used)
  //
#if defined(PALETTE)
  LCD_SetLUTEx(0, PALETTE);
#endif
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void* pData)
{
  int r;

  switch (Cmd) {
    //
    // Required
    //
    case LCD_X_INITCONTROLLER: {
      //
      // Called during the initialization process in order to set up the
      // display controller and put it into operation. If the display
      // controller is not initialized by any external routine this needs
      // to be adapted by the customer...
      //
      _InitController();
      return 0;
    }
    case LCD_X_SETVRAMADDR: {
      //
      // Required for setting the address of the video RAM for drivers
      // with memory mapped video RAM which is passed in the 'pVRAM' element of p
      //
      LCD_X_SETVRAMADDR_INFO* p;
      p = (LCD_X_SETVRAMADDR_INFO*)pData;
      _SetVRAMAddr(p->pVRAM);
      return 0;
    }
    case LCD_X_SETORG: {
      //
      // Required for setting the display origin which is passed in the 'xPos' and 'yPos' element of p
      //
      LCD_X_SETORG_INFO* p;
      p = (LCD_X_SETORG_INFO*)pData;
      _SetOrg(p->xPos, p->yPos);
      return 0;
    }
    case LCD_X_SETLUTENTRY: {
      //
      // Required for setting a lookup table entry which is passed in the 'Pos' and 'Color' element of p
      //
      LCD_X_SETLUTENTRY_INFO* p;
      p = (LCD_X_SETLUTENTRY_INFO*)pData;
      _SetLUTEntry(p->Color, p->Pos);
      return 0;
    }
    case LCD_X_ON: {
      //
      // Required if the display controller should support switching on and off
      //
      return 0;
    }
    case LCD_X_OFF: {
      //
      // Required if the display controller should support switching on and off
      //
      // ...
      return 0;
    }
    default:
      r = -1;
  }
  return r;
}

/*************************** End of file ****************************/

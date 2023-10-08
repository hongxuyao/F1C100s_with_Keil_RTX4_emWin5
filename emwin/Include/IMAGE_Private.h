/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.46 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Software GmbH
Licensed to:              ARM Ltd, 110 Fulbourn Road, CB1 9NJ Cambridge, UK
Licensed SEGGER software: emWin
License number:           GUI-00181
License model:            LES-SLA-20007, Agreement, effective since October 1st 2011 
Licensed product:         MDK-ARM Professional
Licensed platform:        ARM7/9, Cortex-M/R4
Licensed number of seats: -
----------------------------------------------------------------------
File        : IMAGE_Private.h
Purpose     : Private IMAGE include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef IMAGE_PRIVATE_H
#define IMAGE_PRIVATE_H

#include "IMAGE.h"
#include "GUI_Private.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Private config defaults
*
**********************************************************************
*/
#ifndef IMAGE_SUPPORT_TRANSPARENCY
  #define IMAGE_SUPPORT_TRANSPARENCY WM_SUPPORT_TRANSPARENCY
#endif

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  U32 Flags;
} IMAGE_PROPS;

typedef struct {
  WIDGET              Widget;
  IMAGE_PROPS         Props;
  const void        * pData;                                                          // Data pointer of the object to be drawn (Bitmap, BMP, GIF, JPEG, PNG)
  void             (* pfDrawImageAt) (IMAGE_Handle hObj, int xPos, int yPos);         // Object specific (Bitmap, BMP, GIF, JPEG, PNG) code
  void             (* pfPaint)       (IMAGE_Handle hObj);                             // Drawing method specific (Default, Tiled, Magnified) code
  void             (* pfGetImageSize)(IMAGE_Handle hObj, int * pxSize, int * pySize); // Returns the image size of the attached item
  void             (* pfOnTimer)     (IMAGE_Handle hObj);                             // Timer function for animated images (currently only animated GIFs are supported)
  U32                 FileSize;
  //
  // Data items used by IAMGE_GIF.c
  //
  int                 NumImages;    // Number of (sub)images
  int                 CurrentImage; // Image index used for animated images
  GUI_TIMER_HANDLE    hTimer;       // Timer used for animated images
  //
  // Data items used by IAMGE_DTA.c
  //
  GUI_BITMAP          Bitmap;       // Bitmap structure
  GUI_LOGPALETTE      Palette;      // Palette structure
  //
  // Data items used by Image_...Ex() - functions
  //
  void              * pVoid;        // Void pointer passed to GetData() function
  GUI_GET_DATA_FUNC * pfGetData;    // Pointer to GetData() function
  //
  // Data items used if memory devices are available and IMAGE_CF_MEMDEV has been set
  //
  #if GUI_SUPPORT_MEMDEV
    GUI_MEMDEV_Handle hMem;
  #endif
} IMAGE_OBJ;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define IMAGE_INIT_ID(p) (p->Widget.DebugId = IMAGE_ID)
#else
  #define IMAGE_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  IMAGE_OBJ * IMAGE__LockH(IMAGE_Handle h);
  #define IMAGE_LOCK_H(h)   IMAGE__LockH(h)
#else
  #define IMAGE_LOCK_H(h)   (IMAGE_OBJ *)GUI_LOCK_H(h)
#endif

/*********************************************************************
*
*       Module internal data
*
**********************************************************************
*/
extern IMAGE_PROPS IMAGE__DefaultProps;

/*********************************************************************
*
*       Private functions
*
**********************************************************************
*/
void IMAGE__SetWindowSize(IMAGE_Handle hObj);
void IMAGE__FreeAttached (IMAGE_Handle hObj, int LeaveTimer);

#endif // GUI_WINSUPPORT
#endif // IMAGE_PRIVATE_H

/*************************** End of file ****************************/

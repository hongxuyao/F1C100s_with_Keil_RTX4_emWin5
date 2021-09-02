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
File    : Global.h
Purpose : Global types
          In case your application already has a Global.h, you should
          merge the files. In order to use Segger code, the types
          U8, U16, U32, I8, I16, I32 need to be defined in Global.h;
          additional definitions do not hurt.
Revision: $Rev: 9374 $
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GLOBAL_H            // Guard against multiple inclusion
#define GLOBAL_H

#define U8    unsigned char
#define I8    signed char
#define U16   unsigned short
#define I16   signed short
#ifdef __x86_64__
#define U32   unsigned
#define I32   int
#else
#define U32   unsigned long
#define I32   signed long
#endif

#if defined(_WIN32) && !defined(__clang__)
  //
  // Microsoft VC6 compiler related
  //
  #define U64   unsigned __int64
  #define U128  unsigned __int128
  #define I64   __int64
  #define I128  __int128
  #if _MSC_VER <= 1200
    #define U64_C(x) x##UI64
  #else
    #define U64_C(x) x##ULL
  #endif
#else
  //
  // C99 compliant compiler
  //
  #define U64   unsigned long long
  #define I64   signed long long
  #define U64_C(x) x##ULL
#endif

#ifndef PTR_ADDR
  #if (defined(_WIN64) || defined(__LP64__))  // 64-bit symbols used by Visual Studio and GCC, maybe others as well.
    #define PTR_ADDR  U64
  #else
    #define PTR_ADDR  U32
  #endif
#endif

#endif                      // Avoid multiple inclusion

/*************************** End of file ****************************/

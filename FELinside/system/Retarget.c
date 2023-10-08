/******************************************************************************/
/* RETARGET.C: 'Retarget' layer for target-dependent low level functions      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <rt_misc.h>

extern void sys_uart_putc(char c);

#pragma import(__use_no_semihosting)
#pragma import(__use_no_semihosting_swi)

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

static void sendchar(int c)
{
  if (c == '\n')  {
    sys_uart_putc('\r');
  }
  sys_uart_putc(c);
}

int fputc(int ch, FILE* f)
{
  sendchar(ch);
  return (ch);
}

int fgetc(FILE* f)
{
  return (0);
}


int ferror(FILE* f)
{
  /* Your implementation of ferror */
  return EOF;
}


void _ttywrch(int ch)
{
  sendchar(ch);
}


void _sys_exit(int return_code)
{
  while (1);    /* endless loop */
}

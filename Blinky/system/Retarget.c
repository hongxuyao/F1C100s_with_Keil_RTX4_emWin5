// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
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

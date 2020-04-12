// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
#include <RTL.h>
#include <stdio.h>
#include "efxinc.h"
#include "f1c100s-irq.h"
#include "io.h"

/*----------------------------------------------------------------------------
 *      RTX User configuration part BEGIN
 *---------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>Task Configuration
// =====================
//
//   <o>Number of concurrent running tasks <0-250>
//   <i> Define max. number of tasks that will run at the same time.
//   <i> Default: 6
#ifndef OS_TASKCNT
#define OS_TASKCNT      40
#endif

//   <o>Number of tasks with user-provided stack <0-250>
//   <i> Define the number of tasks that will use a bigger stack.
//   <i> The memory space for the stack is provided by the user.
//   <i> Default: 0
#ifndef OS_PRIVCNT
#define OS_PRIVCNT      OS_TASKCNT
#endif

//   <o>Task stack size [defined in U32] <20-4096:8><#/4>
//   <i> Set the stack size for tasks which is assigned by the system.
//   <i> Default: 128
#ifndef OS_STKSIZE
#define OS_STKSIZE      512
#endif

// <q>Check for the stack overflow
// ===============================
// <i> Include the stack checking code for a stack overflow.
// <i> Note that additional code reduces the RTX performance.
#ifndef OS_STKCHECK
#define OS_STKCHECK     0
#endif

// </h>
// <h>Tick Timer Configuration
// =============================
//   <o>Hardware timer <0=> Timer 0 <1=> Timer 1 <2=> Timer 2 <3=> PIT
//   <i> Define the on-chip timer used as a time-base for RTX.
//   <i> Default: Timer 0
#ifndef OS_TIMER
#define OS_TIMER        0
#endif

//   <o>Timer clock value [Hz] <1-1000000000>
//   <i> Set the timer clock value for selected timer.
//   <i> Default: 1000000  (1MHz)
#ifndef OS_CLOCK
#define OS_CLOCK        24000000UL  /* 24Mhz for TIMER0 */
#endif

//   <o>Timer tick value [us] <1-1000000>
//   <i> Set the timer tick value for selected timer.
//   <i> Default: 10000  (10ms)
#ifndef OS_TICK
#define OS_TICK         1000
#endif

// </h>

// <h>System Configuration
// =======================
// <e>Round-Robin Task switching
// =============================
// <i> Enable Round-Robin Task switching.
#ifndef OS_ROBIN
#define OS_ROBIN        1
#endif

//   <o>Round-Robin Timeout [ticks] <1-1000>
//   <i> Define how long a task will execute before a task switch.
//   <i> Default: 5
#ifndef OS_ROBINTOUT
#define OS_ROBINTOUT    5
#endif

// </e>

//   <o>Number of user timers <0-250>
//   <i> Define max. number of user timers that will run at the same time.
//   <i> Default: 0  (User timers disabled)
#ifndef OS_TIMERCNT
#define OS_TIMERCNT     80
#endif

//   <o>ISR FIFO Queue size<4=>   4 entries  <8=>   8 entries
//                         <12=> 12 entries  <16=> 16 entries
//                         <24=> 24 entries  <32=> 32 entries
//                         <48=> 48 entries  <64=> 64 entries
//                         <96=> 96 entries
//   <i> ISR functions store requests to this buffer,
//   <i> when they are called from the IRQ handler.
//   <i> Default: 16 entries
#ifndef OS_FIFOSZ
#define OS_FIFOSZ       96
#endif

// </h>


//------------- <<< end of configuration section >>> -----------------------

// Standard library system mutexes
// ===============================
//  Define max. number system mutexes that are used to protect
//  the arm standard runtime library. For microlib they are not used.
#ifndef OS_MUTEXCNT
#define OS_MUTEXCNT     16
#endif

/*----------------------------------------------------------------------------
 *      RTX User configuration part END
 *---------------------------------------------------------------------------*/
#define TIMER_REG_BASE  (0x01C20C00UL)

/* OS_TRV macro specifies the timer reload value for the peripheral timer.
Peripheral timer counts up to a reload value, then then overflows to 0,
and then generates a tick interrupt. The reload value should be calculated to
generate the desired interval length (for example 10 ms). */
#define OS_TRV          ((U32)(((double)OS_CLOCK*(double)OS_TICK)/1E6)-1)

/*
OS_TVAL macro is used to read the current timer value for a count-up timer.
The RTX kernel uses it to check whether a timer interrupt is a periodic timer interrupt
or a software forced interrupt.  */
#define OS_TVAL         (OS_TRV-read32(TIMER_REG_BASE+0x18))    /*  Timer Value     */

/* OS_TOVF specifies a timer overflow flag.
The RTX kernel uses it together with the OS_TVAL macro to differentiate between
the periodic timer interrupts and forced timer interrupts.  */
#define OS_TOVF         (read32(TIMER_REG_BASE+0x04)&(1<<0))    /*  Reload Flag     */

/* OS_TFIRQ() specifies a code sequence to force a timer interrupt.
This must be a software triggered interrupt if the peripheral timer does not
allow manual setting of an overflow flag. If manual setting is possible,
this macro should set a peripheral timer overflow flag,
which will cause a timer interrupt.  */
#define OS_TFIRQ()      do{f1c100s_intc_force_irq(F1C100S_IRQ_TIMER0);}while(0)

/* OS_TIACK() is used to acknowledge an interrupt from the timer interrupt function
to release the timer interrupt logic.  */
static inline void OS_TIACK(void)
{
  U8 nIRQ = F1C100S_IRQ_TIMER0;
  // 清除TIMER0模块的IRQ状态标志
  write32(TIMER_REG_BASE + 0x04, (1 << 0));
  f1c100s_intc_clear_pend(nIRQ);
}

U8 volatile backup_nirq = 0;

static void timer0_irq(void)
{
  if (read32(TIMER_REG_BASE + 0x04) & (1 << 0)) {
    write32(TIMER_REG_BASE + 0x04, (1 << 0));
  }
}

/* OS_TINIT() macro is used to initialize the peripheral timer/counter,
setup a timer mode, and set a timer reload. Timer interrupts are also
activated here by enabling a peripheral timer interrupt.
This code is executed from the os_sys_init() function.  */
static inline void OS_TINIT(void)
{
  f1c100s_intc_set_isr(F1C100S_IRQ_TIMER0, timer0_irq);
  write32(TIMER_REG_BASE + 0x14, OS_TRV);
  write32(TIMER_REG_BASE + 0x10, 0x04);
  write32(TIMER_REG_BASE + 0x10, read32(TIMER_REG_BASE + 0x10) | (1 << 1));
  while (read32(TIMER_REG_BASE + 0x10) & (1 << 1));
  write32(TIMER_REG_BASE + 0x10, read32(TIMER_REG_BASE + 0x10) | (1 << 0));
  write32(TIMER_REG_BASE + 0x00, (1 << 0));
}

/* OS_LOCK() macro disables timer tick interrupts.
It is used to avoid interrupting the system task scheduler.
This macro should disable both the periodic timer interrupts
and the forced interrupts.
This code is executed from the tsk_lock() function.  */
static inline void OS_LOCK(void)
{
  f1c100s_intc_disable_irq(F1C100S_IRQ_TIMER0);
}

/* OS_UNLOCK() macro enables the timer tick interrupts.
The code sequence specified here should enable the periodic timer interrupts
and the forced interrupts. This code is executed from tsk_unlock() function.  */
static inline void OS_UNLOCK(void)
{
  f1c100s_intc_enable_irq(F1C100S_IRQ_TIMER0);
}

/* WARNING: Using IDLE mode might cause you troubles while debugging. */
#define _idle_()        ;

/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/
extern void os_clock_interrupt(void);
extern void os_def_interrupt(void) __irq;
__asm void IRQ_Handler(void)
{
  /* Common System Interrupt Handler Entry. */
  PRESERVE8
  ARM

  ;// 保存寄存器R0-R3以供下列代码使用
  PUSH    {R0-R3,R12,LR}

  ;// read INTC_VECTOR_REG and save to backup_nirq
  LDR     R1, =0x01C20400   ;// R1=INTC_REG_BASE
  LDR     R0, [R1, #0x00]   ;// read irq number
  LSL     R0, R0, #24
  LSR     R0, R0, #26
  LDR     R2, =__cpp(&backup_nirq)
  STRB    R0, [R2]          ;// R0=nIRQ

  ;// clear forced-irq bit
  LSR     R2, R0, #5
  LSL     R2, R2, #2
  ADD     R1, R1, R2
  LDR     R2, [R1, #0x50]   ;// read fast-forced pending
  MOV     R12, #1
  AND     R3, R0, #0x1F
  BIC     R3, R2, R12, LSL R3
  STR     R3, [R1, #0x50]   ;// write fast-forced pending

  ;// 判别中断号是否作为os_clock的TIMER0
  CMP     R0, #F1C100S_IRQ_TIMER0
  POP     {R0-R3,R12,LR}

  ;// 调用os_def_interrupt()
  LDRNE   PC, =__cpp(os_def_interrupt)

  ;// 调用os_clock_interrupt()
  LDR     PC, =__cpp(os_clock_interrupt)

  ;// 不应该会执行到这里！
  B       .
}

__irq void os_def_interrupt(void)
{
  U8 nIRQ = backup_nirq;
  f1c100s_intc_dispatch(nIRQ);
  f1c100s_intc_clear_pend(nIRQ);
}


/*--------------------------- os_idle_demon ---------------------------------*/

__task void os_idle_demon(void)
{
  /* The idle demon is a system task, running when no other task is ready */
  /* to run. The 'os_xxx' function calls are not allowed from this task.  */
  for (;;) {
    /* HERE: include optional user code to be executed when no task runs.*/
    _idle_();
  }
}

/*--------------------------- os_tmr_call -----------------------------------*/
#if OS_TIMERCNT > 0
/* os_tmr_call()会在Tick中断或Task线程里执行，这是它的设计硬伤。*/
static void UFX_PostTaskTimer(U16 info);
#endif
void os_tmr_call(U16 info)
{
  /* This function is called when the user timer has expired. Parameter   */
  /* 'info' holds the value, defined when the timer was created.          */

  /* HERE: include optional user code to be executed on timeout. */
#if OS_TIMERCNT > 0
  UFX_PostTaskTimer(info);
#endif
}


/*--------------------------- os_error --------------------------------------*/

void os_error(U32 err_code)
{
  /* This function is called when a runtime error is detected. */
  OS_TID err_task;

  switch (err_code) {
    case OS_ERR_STK_OVF:
      /* Identify the task with stack overflow. */
      err_task = isr_tsk_get();
      DBG_PRINT("\nOS_ERR_STK_OVF(%u)\n", err_task);
      break;
    case OS_ERR_FIFO_OVF:
      (void)err_task;
      break;
    case OS_ERR_MBX_OVF:
      (void)err_task;
      break;
  }
  /* End-less loop */
  DBG_PRINT("\nos_error(%u)\n", err_code);
  //for (;;);
}


/*--------------------------- os_timer --------------------------------------*/

#if OS_TIMERCNT > 0

#if OS_TIMERCNT > 255
#error OS_TIMERCNT的取值不能大于255。
#endif

typedef struct {
  void* parg;
  void (*pfn)(void*);
} TskTmrType;

static TskTmrType tsktmr_pool[OS_TIMERCNT];
static os_mbx_declare(mb_tsktmr, OS_TIMERCNT);
#define HDL_MAX OS_TIMERCNT
#include "halloc.h"

static __task void __ThreadTimerWaiter(void)
{
  CPU_SR_DECL;
  void (*pfn)(void*);
  void* parg;
  U32 uival;
  U16 info;
  void* msg;

  for (;;) {
    if (os_mbx_wait(&mb_tsktmr, &msg, TWAIT_FOREVER) != OS_R_TMO) {
      uival = (U32)msg;
      if (uival & (1UL << 31)) {
        info = (U16)uival;
        if (info > 0) {
          parg = tsktmr_pool[info - 1].parg;
          pfn = tsktmr_pool[info - 1].pfn;
          CPU_ENTER_CRITICAL();
          __hdl_free(info);
          CPU_EXIT_CRITICAL();
          if (pfn) {
            pfn(parg);
          }
        }
      }
    }
  }
}

/* 建立线程定时器的基础设施，该函数仅供BSP调用一次 */
void UFX_SetupTaskTimer(void)
{
  static U64 __StackTimerWaiter[1024 / 8] MEM_PI_STACK;
  CPU_SR_DECL;
  OS_TID task_id;

  CPU_ENTER_CRITICAL();
  __hdl_pool_init();
  CPU_EXIT_CRITICAL();

  os_mbx_init(&mb_tsktmr, sizeof(mb_tsktmr));

  task_id = os_tsk_create_user(
                __ThreadTimerWaiter,
                TSK_PRIO_HIGHEST,
                __StackTimerWaiter,
                sizeof(__StackTimerWaiter));

  if (task_id == 0) {
    sys_suspend();
    return;
  }
}

/* 启动一个线程定时器 */
bool_t UFX_AddTaskTimer(U16 dly, void (*pfn)(void*), void* parg)
{
  CPU_SR_DECL;
  OS_ID tmr_id = 0;
  U16 info = 0;

  CPU_ENTER_CRITICAL();
  info = __hdl_alloc();
  CPU_EXIT_CRITICAL();
  if (info == 0) {
    return false;
  }
  tsktmr_pool[info - 1].parg = parg;
  tsktmr_pool[info - 1].pfn = pfn;
  tmr_id = os_tmr_create(dly, info);
  if (tmr_id == NULL) {
    CPU_ENTER_CRITICAL();
    __hdl_free(info);
    CPU_EXIT_CRITICAL();
    return false;
  }
  return true;
}

/* 在RTX的os_tmr_call()中调用即可 */
void UFX_PostTaskTimer(U16 info)
{
  int nfree;
  CPU_SR_DECL;

  if (info > 0) {
    nfree = isr_mbx_check(&mb_tsktmr);
    if (nfree > 0) {
      // 发送定时器事件到定时器专用线程。
      isr_mbx_send(&mb_tsktmr, (void*)((1UL << 31) | info));
    } else {
      // 邮箱已满，只能丢弃该定时器事件。
      CPU_ENTER_CRITICAL();
      __hdl_free(info);
      CPU_EXIT_CRITICAL();
      os_error(OS_ERR_MBX_OVF);
    }
  }
}

#else
#warning 定义OS_TIMERCNT为非零值，以启用UFX_AddTaskTimer()功能！
#endif

/*----------------------------------------------------------------------------
 *      RTX Configuration Functions
 *---------------------------------------------------------------------------*/
#include "RTX_lib.c"

/* 该结构体与异常入口的编代码匹配 */
typedef struct {
  uint32_t R0;
  uint32_t R1;
  uint32_t R2;
  uint32_t R3;
  uint32_t R4;
  uint32_t R5;
  uint32_t R6;
  uint32_t R7;
  uint32_t R8;
  uint32_t R9;
  uint32_t R10;
  uint32_t R11;
  uint32_t R12;
  uint32_t LR;
  uint32_t SPSR;
  uint32_t PC;
} ExceptionFrameType;

void PrintExceptionMessage(ExceptionFrameType* frame, uint32_t cpsr)
{
  DBG_PRINT("R0   = %08X\n", frame->R0);
  DBG_PRINT("R1   = %08X\n", frame->R1);
  DBG_PRINT("R2   = %08X\n", frame->R2);
  DBG_PRINT("R3   = %08X\n", frame->R3);
  DBG_PRINT("R4   = %08X\n", frame->R4);
  DBG_PRINT("R5   = %08X\n", frame->R5);
  DBG_PRINT("R6   = %08X\n", frame->R6);
  DBG_PRINT("R7   = %08X\n", frame->R7);
  DBG_PRINT("R8   = %08X\n", frame->R8);
  DBG_PRINT("R9   = %08X\n", frame->R9);
  DBG_PRINT("R10  = %08X\n", frame->R10);
  DBG_PRINT("R11  = %08X\n", frame->R11);
  DBG_PRINT("R12  = %08X\n", frame->R12);
  DBG_PRINT("SP   = %08X\n", (uint32_t)frame);
  DBG_PRINT("LR   = %08X\n", frame->LR);
  DBG_PRINT("SPSR = %08X\n", frame->SPSR);
  DBG_PRINT("PC   = %08X\n", frame->PC);
  DBG_PRINT("CPSR = %08X\n", cpsr);
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

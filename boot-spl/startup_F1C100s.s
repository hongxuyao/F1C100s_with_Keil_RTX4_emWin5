;// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
Mode_USR        EQU     0x10
Mode_FIQ        EQU     0x11
Mode_IRQ        EQU     0x12
Mode_SVC        EQU     0x13
Mode_ABT        EQU     0x17
Mode_UND        EQU     0x1B
Mode_SYS        EQU     0x1F

I_Bit           EQU     0x80            ; when I bit is set, IRQ is disabled
F_Bit           EQU     0x40            ; when F bit is set, FIQ is disabled
T_Bit           EQU     0x20            ; when T bit is set, CPU is in Thumb mode

USR_Stack_Size  EQU     0x00001000
SVC_Stack_Size  EQU     0x00001000
IRQ_Stack_Size  EQU     0x00001000
FIQ_Stack_Size  EQU     0x00001000
ABT_Stack_Size  EQU     0x00000400
UND_Stack_Size  EQU     0x00000400

ISR_Stack_Size  EQU     (UND_Stack_Size + SVC_Stack_Size + ABT_Stack_Size + \
                         FIQ_Stack_Size + IRQ_Stack_Size)

                AREA    STACK, NOINIT, READWRITE, ALIGN=3

Stack_Mem       SPACE   USR_Stack_Size
__initial_sp    SPACE   ISR_Stack_Size
Stack_Top


;// <h> Heap Configuration
;//   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF>
;// </h>

Heap_Size       EQU     0x400000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8


; Area Definition and Entry Point
;  Startup Code must be linked first at Address at which it expects to run.
                IMPORT  ||Load$$EXE_SPL$$Length||
                IMPORT  ||Load$$LR$$FLASH_LOAD$$Base||
                IMPORT  ||Load$$LR$$FLASH_LOAD$$Limit||

                AREA    RESET, CODE, READONLY
                ARM
                ;Boot head information for BROM booting
BROM_BHI        B       Vectors
                DCB     "eGON.BT0"
                DCD     0, ||Load$$EXE_SPL$$Length||
                DCB     'S', 'P', 'L', 2
                DCD     0, 0
                DCD     0, 0, 0, 0, 0, 0, 0, 0
                DCD     0, 0, 0, 0, 0, 0, 0, 0

Vectors         B       Reset_Handler
                LDR     PC, Undef_Addr
                LDR     PC, SWI_Addr
                LDR     PC, PAbt_Addr
                LDR     PC, DAbt_Addr
                NOP                     ; Reserved Vector
                LDR     PC, IRQ_Addr
                LDR     PC, FIQ_Addr

Reset_Addr      DCD     Reset_Handler
Undef_Addr      DCD     Undef_Handler
SWI_Addr        DCD     SWI_Handler
PAbt_Addr       DCD     PAbt_Handler
DAbt_Addr       DCD     DAbt_Handler
                DCD     0               ; Reserved Address
IRQ_Addr        DCD     IRQ_Handler
FIQ_Addr        DCD     FIQ_Handler

_region_info    DCD     ||Load$$LR$$FLASH_LOAD$$Base||
                DCD     ||Load$$LR$$FLASH_LOAD$$Limit||

                EXPORT  Undef_Handler   [WEAK]
                EXPORT  SWI_Handler     [WEAK]
                EXPORT  PAbt_Handler    [WEAK]
                EXPORT  DAbt_Handler    [WEAK]
                EXPORT  IRQ_Handler     [WEAK]
                EXPORT  FIQ_Handler     [WEAK]
Undef_Handler
SWI_Handler
PAbt_Handler
DAbt_Handler
IRQ_Handler
FIQ_Handler     B       .

                EXPORT  Reset_Handler
                IMPORT  sys_clock_init
                IMPORT  sys_dram_init
                IMPORT  sys_uart_init
                IMPORT  sys_copyself

                ; /* Enter svc mode and mask interrupts */
Reset_Handler   mrs     r0, cpsr
                bic     r0, r0, #0x1f
                orr     r0, r0, #0xd3
                msr     cpsr_c, r0

                ; /* Set vector to the low address */
                mrc     p15, 0, r0, c1, c0, 0
                bic     r0, #(1<<13)
                mcr     p15, 0, r0, c1, c0, 0

                ; /* Copy vector to the correct address */
                adr     r0, Vectors
                mrc     p15, 0, r2, c1, c0, 0
                ands    r2, r2, #(1 << 13)
                ldreq   r1, =0x00000000
                ldrne   r1, =0xffff0000
                ldmia   r0!, {r2-r8, r10}
                stmia   r1!, {r2-r8, r10}
                ldmia   r0!, {r2-r8, r10}
                stmia   r1!, {r2-r8, r10}

                ; /* Initial system clock, ddr add uart */
                bic     sp, sp, #0x07   ;// SP已被BROM设为0x00009FF4
                bl      sys_clock_init
                bl      sys_dram_init
                bl      sys_uart_init

                ; /* Copyself to link address */
                adr     r0, Vectors
                ldr     r1, =Vectors
                cmp     r0, r1
                beq     _symb1
                adr     r0, _region_info
                bl      sys_copyself
_symb1          nop
                nop
                nop

; Setup Stack for each mode ----------------------------------------------------
CPUInitStacks   LDR     R0, =Stack_Top

;  Enter Undefined Instruction Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #UND_Stack_Size

;  Enter Abort Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #ABT_Stack_Size

;  Enter FIQ Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #FIQ_Stack_Size

;  Enter IRQ Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #IRQ_Stack_Size

;  Enter Supervisor Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #SVC_Stack_Size

;  Enter User Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_SYS:OR:I_Bit:OR:F_Bit
                IF      :DEF:__MICROLIB

                EXPORT __initial_sp

                ELSE

                MOV     SP, R0
                SUB     SL, SP, #USR_Stack_Size

                ENDIF

; Enter the C code
                IMPORT  __main
                LDR     R0, =__main
                BX      R0
                B       .

                EXPORT  mmu_clean_dcache
mmu_clean_dcache
                MRC     p15,0,R15,c7,c10,3
                BNE     mmu_clean_dcache
                NOP
                MCR     p15,0,R0,c7,c10,4
                NOP
                MCR     p15,0,R0,c7,c7,0
                BX      LR

                IF      :DEF:__MICROLIB

                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE
; User Initial Stack & Heap
                AREA    |.text|, CODE, READONLY

                IMPORT  __use_no_semihosting
                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + USR_Stack_Size)
                LDR     R2, = (Heap_Mem +      Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

;*******************************************************************************
; 导入__use_realtime_heap标号，意为启用HEAP2模型
;*******************************************************************************
                IMPORT  __use_realtime_heap
                EXPORT  __user_heap_extent
__user_heap_extent      ; 原型声明请看 <rt_misc.h>
                BX      LR

                EXPORT  __user_heap_extend
__user_heap_extend      ; 原型声明请看 <rt_misc.h>
                MOV     R0, #0
                BX      LR

                ENDIF

                END

;// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452

Mode_USR        EQU     0x10
Mode_FIQ        EQU     0x11
Mode_IRQ        EQU     0x12
Mode_SVC        EQU     0x13
Mode_ABT        EQU     0x17
Mode_UND        EQU     0x1B
Mode_SYS        EQU     0x1F


I_Bit           EQU     0x80    ;// IRQ bit
F_Bit           EQU     0x40    ;// FIQ bit
T_Bit           EQU     0x20    ;// Thumb bit

                PRESERVE8

                IMPORT  ||Load$$LR$$FLASH_SPL$$Length||

                AREA    RESET, CODE, READONLY
                ENTRY
                ARM

                ;/* Boot head information for BROM booting */
BROM_BHI        B       Vectors
                DCB     "eGON.BT0"
                DCD     0, ||Load$$LR$$FLASH_SPL$$Length||
                DCB     'S', 'P', 'L', 2
                DCD     0, 0
                DCD     0, 0, 0, 0, 0, 0, 0, 0
                ;/* 0x40 - boot params, 0x54 - entry, 0x58 - fel boot type, 0x5c - dram size */
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

Reset_Handler   ;/* Save boot params to 0x00000040 */
                ldr     r0, =0x00000040
                str     sp, [r0, #0]
                str     lr, [r0, #4]
                mrs     lr, cpsr
                str     lr, [r0, #8]
                mrc     p15, 0, lr, c1, c0, 0
                str     lr, [r0, #12]
                mrc     p15, 0, lr, c1, c0, 0
                str     lr, [r0, #16]
                adr     r1, BROM_BHI
                str     r1, [r0, #20]
                bic     sp, sp, #0x07   ;// SP被BROM设为0x00009FF4，或被FEL设为0x0000B418

                ;/* Check boot type just for FEL */
                mov     r0, #0x00000000
                ldr     r1, [r0, #8]
                ldr     r2, =0x4c45462e ;// 检查magic标记是否有".FEL"信息
                cmp     r1, r2
                bne     _symb1
                mov     r3, #1          ;// 若有".FEL"信息则向地址0x58写入1
                str     r3, [r0, #0x58]
_symb1          nop

                ;/* 确保当前SVC模式，并且禁止IRQ和FIQ */
                mrs     r0, cpsr
                bic     r0, r0, #0x1f
                orr     r0, r0, #0xd3
                msr     cpsr_c, r0

                ;/* Set vector to the low address */
                mrc     p15, 0, r0, c1, c0, 0
                bic     r0, #(1<<13)
                mcr     p15, 0, r0, c1, c0, 0

                ;/* Copy vector to the correct address */
                adr     r0, Vectors
                mrc     p15, 0, r2, c1, c0, 0
                tst     r2, #(1<<13)
                ldreq   r1, =0x00000000
                ldrne   r1, =0xffff0000
                ldmia   r0!, {r2-r8, r10}
                stmia   r1!, {r2-r8, r10}
                ldmia   r0!, {r2-r8, r10}
                stmia   r1!, {r2-r8, r10}

                IMPORT  spl_main
                BL      spl_main
                B       .

                EXPORT  mmu_clean_dcache
mmu_clean_dcache
                mrc     p15, 0, r15, c7, c10, 3
                bne     mmu_clean_dcache
                mcr     p15, 0, r0, c7, c10, 4
                nop
                mcr     p15, 0, r0, c7, c7, 0
                bx      lr

                EXPORT  loop_delay
loop_delay      subs    r0, r0
                bne     loop_delay
                bx      lr

                EXPORT  jump_to_uboot
jump_to_uboot   mov     lr, r0
                bx      lr

                EXPORT  return_to_fel
                ;/* 注意！返回到FEL时，64字节向量表被破坏了 */
return_to_fel   mov     r0, #0x00000004
                adr     r1, _fel_sign
                ldr     r2, [r1, #0]
                str     r2, [r0, #0]
                ldr     r2, [r1, #4]
                str     r2, [r0, #4]

                ldr     r0, =0x00000040
                ldr     sp, [r0, #0]
                ldr     lr, [r0, #4]
                ldr     r1, [r0, #16]
                mcr     p15, 0, r1, c1, c0, 0
                ldr     r1, [r0, #12]
                mcr     p15, 0, r1, c1, c0, 0
                ldr     r1, [r0, #8]
                msr     cpsr_cxsf, r1
                bx      lr

                ALIGN   4
_fel_sign       DCB     "eGON.FEL"

                END

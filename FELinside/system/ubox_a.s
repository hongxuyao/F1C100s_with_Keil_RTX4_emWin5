;// 本文件由洪旭耀设计，而使用权分发权自由属于你！
;// 可以通过QQ联系本人：26750452【24小时在线守候】
;///////////////////////////////////////////////////////////////////////////////
            AREA    |.text|, CODE, READONLY
            ARM
            EXPORT  util_fastloop
; void util_fastloop(ubase_t n);
util_fastloop
            SUBS    R0, R0, #1
            BNE     util_fastloop
            BX      LR

            AREA    |.text|, CODE, READONLY
            ARM
            EXPORT  util_rev32
; u32_t util_rev32(u32_t n);
util_rev32
            MVN     R2, #0x0000FF00
            EOR     R1, R0, R0, ROR #16
            AND     R1, R2, R1, LSR #8
            EOR     R0, R1, R0, ROR #8
            BX      LR

            AREA    |.text|, CODE, READONLY
            ARM
            EXPORT  util_rev
; u32_t util_rev(u32_t n);
util_rev
            EOR     R1, R0, R0, ROR #16
            MOV     R1, R1, LSR #8
            BIC     R1, R1, #0xFF00
            EOR     R0, R1, R0, ROR #8
            BX      LR

            AREA    |.text|, CODE, READONLY
            ARM
            EXPORT  util_rev16
; u16_t util_rev16(u16_t n);
util_rev16
            LSR     R1, R0, #8
            ORR     R0, R1, R0, LSL #8
            BIC     R0, R0, #0xFF0000
            BX      LR

            AREA    |.text|, CODE, READONLY
            ARM
            EXPORT  util_halt
; void util_halt(void);
util_halt
            B       .

            AREA    |.text|, CODE, READONLY
            ARM
            EXPORT  util_getCPSR
; ubase_t util_getCPSR(void);
util_getCPSR
            MRS     R0, CPSR
            BX      LR

            AREA    |.text|, CODE, READONLY
            ARM
            EXPORT  util_enter_critical
; ubase_t util_enter_critical(void);
util_enter_critical
            MRS     R0, CPSR
            ORR     R1, R0, #0xC0   ; Disable IRQ and FIQ
            MSR     CPSR_c, R1
            BX      LR

            AREA    |.text|, CODE, READONLY
            ARM
            EXPORT  util_exit_critical
; void util_exit_critical(ubase_t sr);
util_exit_critical
            MSR     CPSR_c, R0
            BX      LR

            AREA    |.text|, CODE, READONLY
            ARM
            EXPORT  util_disable_interrupt
; void util_disable_interrupt(void);
util_disable_interrupt
            MRS     R0, CPSR
            ORR     R0, R0, #0xC0   ; Disable IRQ and FIQ
            MSR     CPSR_c, R0
            BX      LR

            AREA    |.text|, CODE, READONLY
            ARM
            EXPORT  util_enable_interrupt
; void util_enable_interrupt(void);
util_enable_interrupt
            MRS     R0, CPSR
            BIC     R0, R0, #0x80   ; Enable IRQ only
            MSR     CPSR_c, R0
            BX      LR

;///////////////////////////////////////////////////////////////////////////////
            END


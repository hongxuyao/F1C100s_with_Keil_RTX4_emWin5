;// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
;///////////////////////////////////////////////////////////////////////////////
;// CP15的部分位标志
R1_I        EQU         (1<<12)
R1_C        EQU         (1<<2)
R1_A        EQU         (1<<1)
R1_M        EQU         (1<<0)

;///////////////////////////////////////////////////////////////////////////////
;//====================================
;// MMU Cache/TLB/etc on/off functions
;//====================================
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_EnableICache(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_EnableICache
MMU_EnableICache
            MRC         p15,0,R0,c1,c0,0
            ORR         R0,R0,#R1_I
            MCR         p15,0,R0,c1,c0,0
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_DisableICache(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_DisableICache
MMU_DisableICache
            MRC         p15,0,R0,c1,c0,0
            BIC         R0,R0,#R1_I
            MCR         p15,0,R0,c1,c0,0
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_EnableDCache(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_EnableDCache
MMU_EnableDCache
            MRC         p15,0,R0,c1,c0,0
            ORR         R0,R0,#R1_C
            MCR         p15,0,R0,c1,c0,0
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_DisableDCache(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_DisableDCache
MMU_DisableDCache
            MRC         p15,0,R0,c1,c0,0
            BIC         R0,R0,#R1_C
            MCR         p15,0,R0,c1,c0,0
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_EnableAlignFault(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_EnableAlignFault
MMU_EnableAlignFault
            MRC         p15,0,R0,c1,c0,0
            ORR         R0,R0,#R1_A
            MCR         p15,0,R0,c1,c0,0
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_DisableAlignFault(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_DisableAlignFault
MMU_DisableAlignFault
            MRC         p15,0,R0,c1,c0,0
            BIC         R0,R0,#R1_A
            MCR         p15,0,R0,c1,c0,0
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_EnableMMU(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_EnableMMU
MMU_EnableMMU
            MRC         p15,0,R0,c1,c0,0
            ORR         R0,R0,#R1_M
            MCR         p15,0,R0,c1,c0,0
            NOP
            NOP
            NOP
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_DisableMMU(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_DisableMMU
MMU_DisableMMU
            MRC         p15,0,R0,c1,c0,0
            BIC         R0,R0,#R1_M
            MCR         p15,0,R0,c1,c0,0
            BX          LR

;//=========================
;// Set TTBase
;//=========================
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_SetTTBase(unsigned long base);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_SetTTBase
MMU_SetTTBase
            MCR         p15,0,R0,c2,c0,0
            BX          LR

;//=========================
;// Set Domain
;//=========================
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_SetDomain(unsigned long domain);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_SetDomain
MMU_SetDomain
            MCR         p15,0,R0,c3,c0,0
            BX          LR

;//=========================
;// ICache/DCache functions
;//=========================
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateIDCache(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_InvalidateIDCache
MMU_InvalidateIDCache
            MCR         p15,0,R0,c7,c7,0
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateICache(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_InvalidateICache
MMU_InvalidateICache
            MCR         p15,0,R0,c7,c5,0
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateICacheMVA(unsigned long mva);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_InvalidateICacheMVA
MMU_InvalidateICacheMVA
            MCR         p15,0,R0,c7,c5,1
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_PrefetchICacheMVA(unsigned long mva);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_PrefetchICacheMVA
MMU_PrefetchICacheMVA
            MCR         p15,0,R0,c7,c13,1
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateDCache(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_InvalidateDCache
MMU_InvalidateDCache
            MCR         p15,0,R0,c7,c6,0
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateDCacheMVA(unsigned long mva);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_InvalidateDCacheMVA
MMU_InvalidateDCacheMVA
            MCR         p15,0,R0,c7,c6,1
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_CleanDCacheMVA(unsigned long mva);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_CleanDCacheMVA
MMU_CleanDCacheMVA
            MCR         p15,0,R0,c7,c10,1
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_CleanDCacheIndex(unsigned long index);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_CleanDCacheIndex
MMU_CleanDCacheIndex
            MCR         p15,0,R0,c7,c10,2
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_TestCleanDCache(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_TestCleanDCache
MMU_TestCleanDCache
            MRC         p15,0,R15,c7,c10,3
            BNE         MMU_TestCleanDCache
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_DrainWriteBuffer(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_DrainWriteBuffer
MMU_DrainWriteBuffer
            MCR         p15,0,R0,c7,c10,4
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_CleanAndInvalidateDCacheMVA(unsigned long mva);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_CleanAndInvalidateDCacheMVA
MMU_CleanAndInvalidateDCacheMVA
            MCR         p15,0,R0,c7,c14,1
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_CleanAndInvalidateDCacheIndex(unsigned long index);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_CleanAndInvalidateDCacheIndex
MMU_CleanAndInvalidateDCacheIndex
            MCR         p15,0,R0,c7,c14,2
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_TestCleanAndInvalidateDCache(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_TestCleanAndInvalidateDCache
MMU_TestCleanAndInvalidateDCache
            MRC         p15,0,R15,c7,c14,3
            BNE         MMU_TestCleanAndInvalidateDCache
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_WaitForInterrupt(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_WaitForInterrupt
MMU_WaitForInterrupt
            MCR         p15,0,R0,c7,c0,4
            BX          LR

;//===============
;// TLB functions
;//===============
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateTLB(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_InvalidateTLB
MMU_InvalidateTLB
            MCR         p15,0,R0,c8,c7,0
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateTLBMVA(unsigned long mva);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_InvalidateTLBMVA
MMU_InvalidateTLBMVA
            MCR         p15,0,R0,c8,c7,1
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateITLB(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_InvalidateITLB
MMU_InvalidateITLB
            MCR         p15,0,R0,c8,c5,0
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateITLBMVA(unsigned long mva);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_InvalidateITLBMVA
MMU_InvalidateITLBMVA
            MCR         p15,0,R0,c8,c5,1
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateDTLB(void);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_InvalidateDTLB
MMU_InvalidateDTLB
            MCR         p15,0,R0,c8,c6,0
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateDTLBMVA(unsigned long mva);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_InvalidateDTLBMVA
MMU_InvalidateDTLBMVA
            MCR         p15,0,R0,c8,c6,1
            BX          LR


;//=================
;// Cache lock down
;//=================
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_SetDCacheLockdownBase(unsigned long base);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_SetDCacheLockdownBase
MMU_SetDCacheLockdownBase
            ;// R0= victim & lockdown base
            MCR         p15,0,R0,c9,c0,0
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_SetICacheLockdownBase(unsigned long base);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_SetICacheLockdownBase
MMU_SetICacheLockdownBase
            ;// R0= victim & lockdown base
            MCR         p15,0,R0,c9,c0,1
            BX          LR

;//=================
;// TLB lock down
;//=================
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_SetDTLBLockdown(unsigned long baseVictim);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_SetDTLBLockdown
MMU_SetDTLBLockdown
            MCR         p15,0,R0,c10,c0,0
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
;// void MMU_SetITLBLockdown(unsigned long baseVictim);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_SetITLBLockdown
MMU_SetITLBLockdown
            MCR         p15,0,R0,c10,c0,1
            BX          LR

;//============
;// Process ID
;//============
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_SetProcessId(unsigned long pid);
;///////////////////////////////////////////////////////////////////////////////
            AREA        |.text|, CODE, READONLY
            ARM
            EXPORT      MMU_SetProcessId
MMU_SetProcessId
            MCR         p15,0,R0,c13,c0,0
            BX          LR

;///////////////////////////////////////////////////////////////////////////////
            END




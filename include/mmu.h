// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
#ifndef __ARM926EJS_MMU_H__
#define __ARM926EJS_MMU_H__
////////////////////////////////////////////////////////////////////////////////
#include "ftypes.h"

#define CACHE_ALIGN         (32)        // ARM926EJ每cache行32字节(4way)
#define CACHE_ALIGN_SIZ(n)  ((((n)+CACHE_ALIGN-1)/CACHE_ALIGN)*CACHE_ALIGN)

////////////////////////////////////////////////////////////////////////////////
// CP15的部分位标志
#define R1_I                (1<<12)
#define R1_C                (1<< 2)
#define R1_A                (1<< 1)
#define R1_M                (1<< 0)

/* it is used for 1MB virtual page */
#define MMU_ATTRIB_SEC      (0x12)      // Section Descriptor
/* C and B bits for Section descriptor */
#define MMU_ATTRIB_CB       (3<<2)      // DCache_on, write_back
#define MMU_ATTRIB_CNB      (2<<2)      // DCache_on, write_through
#define MMU_ATTRIB_NCB      (1<<2)      // DCache_off, WR_BUF on
#define MMU_ATTRIB_NCNB     (0<<2)      // DCache_off, WR_BUF off
/* Access permission bits (AP) */
#define MMU_ATTRIB_RW       (3<<10)     // supervisor=RW, user=RW
#define MMU_ATTRIB_RO       (2<<10)     // supervisor=RW, user=RO
#define MMU_ATTRIB_FAULT    (1<<10)     // supervisor=RW, user=No Access
/* Config data for Section's domain segment */
#define MMU_ATTRIB_DOM0     (0 <<5)     // Domain index =0
#define MMU_ATTRIB_DOM14    (14<<5)     // Domain index =14
#define MMU_ATTRIB_DOM15    (15<<5)     // Domain index =15
/* Access control bits in Domain access control register */
#define MMU_DOMAIN_FAULT    (0UL)       // 总是产生异常
#define MMU_DOMAIN_CHK      (1UL)       // 检查访问权限
#define MMU_DOMAIN_NOTCHK   (3UL)       // 不检查权限
#define MMU_DOMAIN_VAL      ((MMU_DOMAIN_NOTCHK<<30)|(MMU_DOMAIN_CHK<<28))  //domain15=not-check;domain14=check;others=fault.
/* General bits setting for bootloader 1MB virtual page */
#define MMU_RW_CB           (MMU_ATTRIB_RW|MMU_ATTRIB_DOM14|MMU_ATTRIB_CB|MMU_ATTRIB_SEC)
#define MMU_RW_CNB          (MMU_ATTRIB_RW|MMU_ATTRIB_DOM14|MMU_ATTRIB_CNB|MMU_ATTRIB_SEC)
#define MMU_RW_NCNB         (MMU_ATTRIB_RW|MMU_ATTRIB_DOM14|MMU_ATTRIB_NCNB|MMU_ATTRIB_SEC)
#define MMU_RW_FAULT        (MMU_ATTRIB_RW|MMU_ATTRIB_DOM0|MMU_ATTRIB_NCNB|MMU_ATTRIB_SEC)

//====================================
// MMU Cache/TLB/etc on/off functions
//====================================
void MMU_EnableICache(void);
void MMU_DisableICache(void);
void MMU_EnableDCache(void);
void MMU_DisableDCache(void);
void MMU_EnableAlignFault(void);
void MMU_DisableAlignFault(void);
void MMU_EnableMMU(void);
void MMU_DisableMMU(void);

//=========================
// Set TTBase
//=========================
void MMU_SetTTBase(unsigned long base);
void MMU_SetDomain(unsigned long domain);

//=========================
// ICache/DCache functions
//=========================
void MMU_InvalidateIDCache(void);
void MMU_InvalidateICache(void);
void MMU_InvalidateICacheMVA(unsigned long mva);
void MMU_PrefetchICacheMVA(unsigned long mva);
void MMU_InvalidateDCache(void);
void MMU_InvalidateDCacheMVA(unsigned long mva);
void MMU_CleanDCacheMVA(unsigned long mva);
void MMU_CleanDCacheIndex(unsigned long index);
void MMU_TestCleanDCache(void);
void MMU_DrainWriteBuffer(void);
void MMU_CleanAndInvalidateDCacheMVA(unsigned long mva);
void MMU_CleanAndInvalidateDCacheIndex(unsigned long index);
void MMU_TestCleanAndInvalidateDCache(void);
void MMU_WaitForInterrupt(void);

//===============
// TLB functions
//===============
void MMU_InvalidateTLB(void);
void MMU_InvalidateTLBMVA(unsigned long mva);
void MMU_InvalidateITLB(void);
void MMU_InvalidateITLBMVA(unsigned long mva);
void MMU_InvalidateDTLB(void);
void MMU_InvalidateDTLBMVA(unsigned long mva);

//=================
// Cache lock down
//=================
void MMU_SetDCacheLockdownBase(unsigned long base);
void MMU_SetICacheLockdownBase(unsigned long base);

//=================
// TLB lock down
//=================
void MMU_SetDTLBLockdown(unsigned long baseVictim);
void MMU_SetITLBLockdown(unsigned long baseVictim);

//============
// Process ID
//============
void MMU_SetProcessId(unsigned long pid);

//============
// 自定义MMU函数
//============
void MMU_CleanDCacheArray(unsigned long mva, unsigned long num);
void MMU_InvalidateDCacheArray(unsigned long mva, unsigned long num);

////////////////////////////////////////////////////////////////////////////////
#endif /* __ARM926EJS_MMU_H__ */


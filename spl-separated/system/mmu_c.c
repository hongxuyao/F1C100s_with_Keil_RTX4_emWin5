// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
#include "mmu.h"

#ifndef CACHE_ALIGN
#define CACHE_ALIGN   (32)
#endif

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: MMU_CleanDCacheArray
//| 功能描述 |: 将指定数组所在的cache清理到物理存储器
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |: 给定的数组必须是cacheline为基本单元！
//|          |: 即：32字节对齐，大小是32字节的整数倍
////////////////////////////////////////////////////////////////////////////////
void MMU_CleanDCacheArray(unsigned long mva, unsigned long num)
{
  signed long size = num;
  while (size > 0) {
    MMU_CleanDCacheMVA(mva);
    mva += CACHE_ALIGN;
    size -= CACHE_ALIGN;
  }
  MMU_DrainWriteBuffer();
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: MMU_InvalidateDCacheArray
//| 功能描述 |: 将指定数组所在的cache设为无效，这会导致cache重新填充
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |: 给定的数组必须是cacheline为基本单元！
//|          |: 即：32字节对齐，大小是32字节的整数倍
////////////////////////////////////////////////////////////////////////////////
void MMU_InvalidateDCacheArray(unsigned long mva, unsigned long num)
{
  signed long size = num;
  while (size > 0) {
    MMU_InvalidateDCacheMVA(mva);
    mva += CACHE_ALIGN;
    size -= CACHE_ALIGN;
  }
}


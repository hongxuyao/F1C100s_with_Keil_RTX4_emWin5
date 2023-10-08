// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
////////////////////////////////////////////////////////////////////////////////
#if (defined(HDL_MAX) && (HDL_MAX > 0))

#ifdef __cplusplus
extern "C"  {
#endif

#ifndef __INLINE
#define __INLINE inline
#endif

#undef HANDLE_T

#if (HDL_MAX > 65535)
#define HANDLE_T uint32_t
#elif (HDL_MAX > 255)
#define HANDLE_T uint16_t
#else
#define HANDLE_T uint8_t
#endif

static HANDLE_T hdlpool[HDL_MAX + 1];

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __hdl_pool_init
//| 功能描述 |: 初始化本地模块的句柄池。
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static __INLINE void __hdl_pool_init(void)
{
  unsigned i;

  for (i = 0; i < HDL_MAX; i++) {
    hdlpool[i] = (i + 1);
  }
  hdlpool[i] = 0;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __hdl_alloc
//| 功能描述 |: 从句柄池取出一个空闲句柄。
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static __INLINE HANDLE_T __hdl_alloc(void)
{
  HANDLE_T hdl;

  hdl = hdlpool[0];
  if (hdl == 0)
    return 0;
  hdlpool[0] = hdlpool[hdl];
  return hdl;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __hdl_free
//| 功能描述 |: 释放指定句柄。
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static __INLINE void __hdl_free(HANDLE_T hdl)
{
  hdlpool[hdl] = hdlpool[0];
  hdlpool[0] = hdl;
}

#undef HANDLE_T
#undef HDL_MAX

#ifdef __cplusplus
}
#endif

#endif /* HDL_MAX */


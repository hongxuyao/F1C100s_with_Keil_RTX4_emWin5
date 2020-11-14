#ifndef __F1C100S_IO_H__
#define __F1C100S_IO_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static __inline uint8_t read8(uint32_t addr)
{
  return (*((volatile uint8_t*)(addr)));
}

static __inline uint16_t read16(uint32_t addr)
{
  return (*((volatile uint16_t*)(addr)));
}

static __inline uint32_t read32(uint32_t addr)
{
  return (*((volatile uint32_t*)(addr)));
}

static __inline uint64_t read64(uint32_t addr)
{
  return (*((volatile uint64_t*)(addr)));
}

static __inline void write8(uint32_t addr, uint8_t value)
{
  *((volatile uint8_t*)(addr)) = value;
}

static __inline void write16(uint32_t addr, uint16_t value)
{
  *((volatile uint16_t*)(addr)) = value;
}

static __inline void write32(uint32_t addr, uint32_t value)
{
  *((volatile uint32_t*)(addr)) = value;
}

static __inline void write64(uint32_t addr, uint64_t value)
{
  *((volatile uint64_t*)(addr)) = value;
}

#ifdef __cplusplus
}
#endif

#endif /* __F1C100S_IO_H__ */

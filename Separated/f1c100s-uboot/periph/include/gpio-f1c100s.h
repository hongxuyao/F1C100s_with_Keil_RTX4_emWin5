#ifndef __GPIO_F1C100S_H__
#define __GPIO_F1C100S_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
  GPIO_PULL_UP      = 0,
  GPIO_PULL_DOWN      = 1,
  GPIO_PULL_NONE      = 2,
} gpio_pull_t;

typedef enum {
  GPIO_DRV_WEAK     = 0,
  GPIO_DRV_WEAKER     = 1,
  GPIO_DRV_STRONGER   = 2,
  GPIO_DRV_STRONG     = 3,
} gpio_drv_t;

typedef enum {
  GPIO_RATE_SLOW      = 0,
  GPIO_RATE_FAST      = 1,
} gpio_rate_t;

typedef enum {
  GPIO_DIRECTION_INPUT  = 0,
  GPIO_DIRECTION_OUTPUT = 1,
} gpio_direction_t;

typedef struct {
  uint32_t virt;
  int32_t base;
  int32_t ngpio;
  int32_t oirq;
} gpio_port_t;

extern const gpio_port_t GPIO_PA;
extern const gpio_port_t GPIO_PB;
extern const gpio_port_t GPIO_PC;
extern const gpio_port_t GPIO_PD;
extern const gpio_port_t GPIO_PE;

void gpio_f1c100s_set_cfg(const gpio_port_t* port, int32_t pin, int32_t cfg);
int32_t gpio_f1c100s_get_cfg(const gpio_port_t* port, int32_t pin);
void gpio_f1c100s_set_pull(const gpio_port_t* port, int32_t pin, gpio_pull_t pull);
gpio_pull_t gpio_f1c100s_get_pull(const gpio_port_t* port, int32_t pin);
void gpio_f1c100s_set_drv(const gpio_port_t* port, int32_t pin, gpio_drv_t drv);
gpio_drv_t gpio_f1c100s_get_drv(const gpio_port_t* port, int32_t pin);
void gpio_f1c100s_set_rate(const gpio_port_t* port, int32_t pin, gpio_rate_t rate);
gpio_rate_t gpio_f1c100s_get_rate(const gpio_port_t* port, int32_t pin);
void gpio_f1c100s_set_dir(const gpio_port_t* port, int32_t pin, gpio_direction_t dir);
gpio_direction_t gpio_f1c100s_get_dir(const gpio_port_t* port, int32_t pin);
void gpio_f1c100s_set_value(const gpio_port_t* port, int32_t pin, int32_t value);
int32_t gpio_f1c100s_get_value(const gpio_port_t* port, int32_t pin);
int32_t gpio_f1c100s_to_irq(const gpio_port_t* port, int32_t pin);

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_H__ */

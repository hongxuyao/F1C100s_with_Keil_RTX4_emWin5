#ifndef __PWM_H__
#define __PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
  uint32_t virt;
  char* clk;
  int32_t duty;
  int32_t period;
  int32_t channel;
  int32_t polarity;
  const gpio_port_t* pwm_port;
  int32_t pwm_pin;
  int32_t pwm_pin_cfg;
} pwm_t;

int32_t pwm_f1c100s_init(pwm_t* pwm);
void pwm_f1c100s_config(pwm_t* pwm, int32_t duty, int32_t period, int32_t polarity);
void pwm_f1c100s_set_duty(pwm_t* pwm, int32_t duty);
inline int32_t pwm_f1c100s_get_duty(pwm_t* pwm) {return pwm->duty;}
void pwm_f1c100s_enable(pwm_t* pwm);
void pwm_f1c100s_disable(pwm_t* pwm);

#ifdef __cplusplus
}
#endif

#endif /* __PWM_H__ */

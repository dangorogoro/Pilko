#ifndef MY_SERVO_CON_H
#define MY_SERVO_CON_H
#include "mine.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
typedef struct{
  uint32_t duty;
  uint32_t arm_len;
  uint32_t shaft_len;
}MotorUnit;
void mcpwm_example_gpio_initialize();
uint32_t servo_per_degree_init(uint32_t degree_of_rotation);
void mcpwm_example_servo_control(void *arg);
void servo_control_task(void* arg);
#endif

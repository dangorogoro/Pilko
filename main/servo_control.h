#ifndef MY_SERVO_CON_H
#define MY_SERVO_CON_H
#include "mine.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
void mcpwm_example_gpio_initialize();
uint32_t servo_per_degree_init(uint32_t degree_of_rotation);
void mcpwm_example_servo_control(void *arg);
#endif

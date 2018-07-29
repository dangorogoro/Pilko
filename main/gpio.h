#ifndef MY_GPIO_H
#define MY_GPIO_H
#include "mine.h"
#include "driver/gpio.h"
void charlie_plexing(void *arg);
void led_config();
void led_blink(void *arg);
extern uint32_t led_status;
#endif

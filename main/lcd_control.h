#ifndef MY_LCD_H
#define MY_LCD_H
#include "mine.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#define X_READ 0x94
#define Y_READ 0xd4
void setup_lcd();
uint16_t read_value(uint16_t port);
#endif

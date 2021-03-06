#ifndef MY_LCD_H
#define MY_LCD_H
#include "mine.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#define X_READ 0x94
#define Y_READ 0xd4
typedef struct{
  uint8_t flag;
  int16_t x;
  int16_t y;
}Coordinate;
extern Coordinate target_coord;
void lcd_config();
uint16_t ret_point(uint16_t port);
void lcd_task(void *arg);
Coordinate ret_coordinate();
#endif

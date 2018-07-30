#include "mine.h"

#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22

#define X_MIN 1516
#define X_MAX 2682
#define X_LEN (X_MAX - X_MIN)
#define X_CENTER ((X_MIN + X_MAX) / 2)
#define Y_MIN 1070
#define Y_MAX 3050
#define Y_LEN (Y_MAY - Y_MIN)
#define Y_CENTER ((Y_MIN + Y_MAX) / 2)
/*
 * <------------ x
 *----------------
C*
o*
n*
n*
e*
c*
t*
o*
r*
 *
 *
 *
y*
 * 
 * *
*/

spi_device_handle_t m_spi = NULL;
void lcd_spi_pre_transfer_callback(spi_transaction_t *t){
  //int dc=(int)t->user;
  //gpio_set_level(PIN_NUM_DC, dc);
}
void lcd_config(){
  esp_err_t ret;
  spi_bus_config_t buscfg = {
    .miso_io_num = PIN_NUM_MISO,
    .mosi_io_num = PIN_NUM_MOSI,
    .sclk_io_num = PIN_NUM_CLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
  };
  spi_device_interface_config_t devcfg = {
    .clock_speed_hz = 1024 * 1024,
    .mode = 0,
    .spics_io_num = PIN_NUM_CS,
    //.pre_cb = lcd_spi_pre_transfer_callback
    .queue_size = 7,
  };
  ret = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
  assert(ret==ESP_OK);
  ret = spi_bus_add_device(HSPI_HOST, &devcfg, &m_spi);
  assert(ret==ESP_OK);
}
uint16_t ret_point(uint16_t port){
  esp_err_t ret;
  uint8_t data[3] = {0};
  data[0] = port;
  spi_transaction_t t = {
    .length = 1 * 8 * 3,              // Len is in bytes, transaction length is in bits.
    .tx_buffer = &data,                // Data
    .flags = SPI_TRANS_USE_RXDATA,
  };
  ret = spi_device_transmit(m_spi, &t); //Transmit!
  assert(ret == ESP_OK);              // Should have had no issues.
  return (t.rx_data[1] << 8 | t.rx_data[2]) >> 3;
}
Coordinate ret_coordinate(){
  Coordinate point;
  point.x = ret_point(X_READ);
  point.y = ret_point(Y_READ);
  if(point.x >= X_MAX || point.y <= Y_MIN) point.flag = false;
  else{
    printf("ball find! X--%d Y--%d\n", point.x, point.y);
    point.flag = true;
  }
  point.x -= X_MIN;
  point.y -= Y_MIN;
  return point;
}
void lcd_task(void *arg){
  lcd_config();
  while(1){
    Pilko.coord = ret_coordinate();
	  vTaskDelay(1 / portTICK_RATE_MS);
  }
}

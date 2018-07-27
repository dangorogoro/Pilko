#include "mine.h"

#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22

spi_device_handle_t m_spi = NULL;
void lcd_spi_pre_transfer_callback(spi_transaction_t *t){
    //int dc=(int)t->user;
    //gpio_set_level(PIN_NUM_DC, dc);
}
void setup_lcd(){
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
uint16_t read_value(uint16_t port){
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

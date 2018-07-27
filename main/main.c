#include "mine.h"

void app_main(){
	printf("hello world!!");
	/* Print chip information */
	led_config();
	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);
	printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
	  chip_info.cores,
	  (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
	  (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

  printf("silicon revision %d, ", chip_info.revision);
  
  printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
	  (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

  printf("Testing servo motor.......\n");
  //xTaskCreate(mcpwm_example_servo_control, "mcpwm_example_servo_control", 4096, NULL, 5, NULL);
  //xTaskCreate(led_blink, "led_blink", 4096, NULL, 2, NULL);
  setup_lcd();
	while(1){
	  vTaskDelay(100 / portTICK_RATE_MS);
    led_status = (led_status + 1) % 4;
    printf("X-Position %d, Y-Position %d\r\n", read_value(X_READ), read_value(Y_READ));
	}
	esp_restart();
}

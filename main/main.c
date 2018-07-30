#include "mine.h"
void app_main(){
	/* Print chip information */
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
  xTaskCreate(servo_control_task, "servo_task", 4096, NULL, 1, NULL);
  xTaskCreate(lcd_task, "lcd_task", 4096, NULL, 2, NULL);
  //xTaskCreate(led_blink, "led_blink", 4096, NULL, 2, NULL);
  //xTaskCreate(charlie_plexing, "charlie_plexing", 8192, NULL, 3, NULL);
  uint32_t count = 0;
  while(1){
    count = (count + 1) % 20;
    //led_status = 1 << count;
    led_status = 0xfffff;
    //vTaskDelay(500 / portTICK_RATE_MS);
  }
  esp_restart();
}

#include "mine.h"
#define GPIO_INPUT_IO_0     4
#define GPIO_INPUT_IO_1     5
#define GPIO_INPUT_PIN_SEL  ((1<<GPIO_INPUT_IO_0) | (1<<GPIO_INPUT_IO_1))
#define ESP_INTR_FLAG_DEFAULT 0

#define LED_OUTPUT_RED 16
#define LED_OUTPUT_BLUE 17 
#define LED_OUTPUT_SEL  ((1<<LED_OUTPUT_RED) | (1<<LED_OUTPUT_BLUE))

#define CHARLIE_A_PIN 32
#define CHARLIE_B_PIN 33
#define CHARLIE_C_PIN 14
#define CHARLIE_D_PIN 12
#define CHARLIE_E_PIN 21

#define CHARLIE_A GPIO_SEL_32
#define CHARLIE_B GPIO_SEL_33
#define CHARLIE_C GPIO_SEL_14
#define CHARLIE_D GPIO_SEL_12
#define CHARLIE_E GPIO_SEL_21
#define CHARLIE_SEL (CHARLIE_A | CHARLIE_B | CHARLIE_C | CHARLIE_D | CHARLIE_E)

uint32_t led_status = 0;
static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

void led_config(){
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = LED_OUTPUT_SEL;
  io_conf.pull_down_en = 0;
  io_conf.pull_up_en = 0;
  gpio_config(&io_conf);
}
void charlie_config(){
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT_OD;
  io_conf.pin_bit_mask = CHARLIE_SEL;
  io_conf.pull_down_en = 0;
  io_conf.pull_up_en = 0;
  gpio_config(&io_conf);
}
void led_blink(void *arg){
  led_config();
	uint8_t cnt = 0;
  while(1){
	  //gpio_config_t io_conf;
	  //io_conf.mode = GPIO_MODE_OUTPUT;
	  //io_conf.pin_bit_mask = 1<<LED_OUTPUT_RED;
	  //gpio_config(&io_conf);
	  if(led_status == 0){
      //gpio_config_t io_conf;
      //io_conf.mode = GPIO_MODE_OUTPUT_OD;
      //io_conf.pin_bit_mask = 1<<LED_OUTPUT_RED;
      //gpio_config(&io_conf);
      gpio_set_level(LED_OUTPUT_RED, 1);
      gpio_set_level(LED_OUTPUT_BLUE,1);
	  }
	  else if(led_status == 1){
      gpio_set_level(LED_OUTPUT_RED, 1);
      gpio_set_level(LED_OUTPUT_BLUE,0);
	  }
	  else if(led_status == 2){
      gpio_set_level(LED_OUTPUT_RED, 0);
      gpio_set_level(LED_OUTPUT_BLUE,1);
	  }
	  else if(led_status == 3){
      gpio_set_level(LED_OUTPUT_RED, (cnt + 1) % 2);
      gpio_set_level(LED_OUTPUT_BLUE,cnt % 2);
	    vTaskDelay(1 / portTICK_RATE_MS);
	    cnt++;
	  }
  }
}
void charlie_set_low_cont(uint8_t i){
  switch(i){
    case 0:
      gpio_set_level(CHARLIE_A_PIN, 0);
      break;
    case 1:
      gpio_set_level(CHARLIE_B_PIN, 0);
      break;
    case 2:
      gpio_set_level(CHARLIE_C_PIN, 0);
      break;
    case 3:
      gpio_set_level(CHARLIE_D_PIN, 0);
      break;
    case 4:
      gpio_set_level(CHARLIE_E_PIN, 0);
      break;
  }
}
void charlie_set_out_mode(uint8_t pin, uint8_t status){
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT_OD;
  io_conf.pin_bit_mask = 0;
  io_conf.pull_down_en = 0;
  io_conf.pull_up_en = 0;
  printf("select_mode %d, i %d\n",pin, status);
  uint8_t flag = 0;
  for(int i = 0; i < 4; i++){
    if(((pin >> i) & 0x01) == 0){
      flag = 1;
      switch(status){
        case 0:
          if(i == 0) io_conf.pin_bit_mask |= CHARLIE_B;
          else if(i == 1) io_conf.pin_bit_mask |= CHARLIE_C;
          else if(i == 2) io_conf.pin_bit_mask |= CHARLIE_D;
          else if(i == 3) io_conf.pin_bit_mask |= CHARLIE_E;
          break;
        case 1:
          if(i == 0) io_conf.pin_bit_mask |= CHARLIE_A;
          else if(i == 1) io_conf.pin_bit_mask |= CHARLIE_C;
          else if(i == 2) io_conf.pin_bit_mask |= CHARLIE_D;
          else if(i == 3) io_conf.pin_bit_mask |= CHARLIE_E;
          break;
        case 2:
          if(i == 0) io_conf.pin_bit_mask |= CHARLIE_A;
          else if(i == 1) io_conf.pin_bit_mask |= CHARLIE_B;
          else if(i == 2) io_conf.pin_bit_mask |= CHARLIE_D;
          else if(i == 3) io_conf.pin_bit_mask |= CHARLIE_E;
          break;
        case 3:
          if(i == 0) io_conf.pin_bit_mask |= CHARLIE_A;
          else if(i == 1) io_conf.pin_bit_mask |= CHARLIE_B;
          else if(i == 2) io_conf.pin_bit_mask |= CHARLIE_C;
          else if(i == 3) io_conf.pin_bit_mask |= CHARLIE_E;
          break;
        case 4:
          if(i == 0) io_conf.pin_bit_mask |= CHARLIE_A;
          else if(i == 1) io_conf.pin_bit_mask |= CHARLIE_B;
          else if(i == 2) io_conf.pin_bit_mask |= CHARLIE_C;
          else if(i == 3) io_conf.pin_bit_mask |= CHARLIE_D;
          break;
      }
    }
  }
  if(flag == 1) gpio_config(&io_conf);
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = 0;
  flag = 0;
  for(int i = 0; i < 4; i++){
    if(((pin >> i) & 0x01) == 1){
      flag = 1;
      switch(status){
        case 0:
          if(i == 0) io_conf.pin_bit_mask |= CHARLIE_B;
          else if(i == 1) io_conf.pin_bit_mask |= CHARLIE_C;
          else if(i == 2) io_conf.pin_bit_mask |= CHARLIE_D;
          else if(i == 3) io_conf.pin_bit_mask |= CHARLIE_E;
          break;
        case 1:
          if(i == 0) io_conf.pin_bit_mask |= CHARLIE_A;
          else if(i == 1) io_conf.pin_bit_mask |= CHARLIE_C;
          else if(i == 2) io_conf.pin_bit_mask |= CHARLIE_D;
          else if(i == 3) io_conf.pin_bit_mask |= CHARLIE_E;
          break;
        case 2:
          if(i == 0) io_conf.pin_bit_mask |= CHARLIE_A;
          else if(i == 1) io_conf.pin_bit_mask |= CHARLIE_B;
          else if(i == 2) io_conf.pin_bit_mask |= CHARLIE_D;
          else if(i == 3) io_conf.pin_bit_mask |= CHARLIE_E;
          break;
        case 3:
          if(i == 0) io_conf.pin_bit_mask |= CHARLIE_A;
          else if(i == 1) io_conf.pin_bit_mask |= CHARLIE_B;
          else if(i == 2) io_conf.pin_bit_mask |= CHARLIE_C;
          else if(i == 3) io_conf.pin_bit_mask |= CHARLIE_E;
          break;
        case 4:
          if(i == 0) io_conf.pin_bit_mask |= CHARLIE_A;
          else if(i == 1) io_conf.pin_bit_mask |= CHARLIE_B;
          else if(i == 2) io_conf.pin_bit_mask |= CHARLIE_C;
          else if(i == 3) io_conf.pin_bit_mask |= CHARLIE_D;
          break;
      }
    }
  }
  if(flag == 1) gpio_config(&io_conf);
}
void charlie_set_all(uint8_t status){
  gpio_set_level(CHARLIE_A_PIN, status);
  gpio_set_level(CHARLIE_B_PIN, status);
  gpio_set_level(CHARLIE_C_PIN, status);
  gpio_set_level(CHARLIE_D_PIN, status);
  gpio_set_level(CHARLIE_E_PIN, status);
}
  
void charlie_plexing(void *arg){
  uint8_t select_pin[5];
  while(1){
    charlie_set_all(0);
    printf("pa\n");
    //charlie_config();
    for(int i = 0;i < 5; i++){
      charlie_set_all(0);
      printf("led_status is 0x%x\n",led_status);
      select_pin[i] = (uint8_t)((led_status & (0xf << 4*i)) >> 4*i);
      charlie_set_out_mode(select_pin[i], i);
      printf("po\n");
      charlie_set_all(1);
      charlie_set_low_cont(i);
      printf("pi\n");
      vTaskDelay(50 / portTICK_RATE_MS);
    }
      vTaskDelay(1 / portTICK_RATE_MS);
  }
}

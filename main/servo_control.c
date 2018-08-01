#include "mine.h"

#define SERVO_MIN_PULSEWIDTH 600 //Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH 2400 //Maximum pulse width in microsecond
#define SERVO_MED_PULSEWIDTH ((SERVO_MIN_PULSEWIDTH + SERVO_MAX_PULSEWIDTH) / 2)
#define SERVO_MAX_DEGREE 180 //Maximum angle in degree upto which servo can rotate

#define MCPWM_GPIO_A 18
#define MCPWM_GPIO_B 26
#define MCPWM_GPIO_C 27
void motor_control(int32_t motor_select, uint32_t angle){
  uint32_t duty = servo_per_degree_init(angle);
  switch(motor_select){
    case MCPWM_GPIO_A:
      mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty);
      break;
    case MCPWM_GPIO_B:
      mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, duty);
      break;
    case MCPWM_GPIO_C:
      mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, duty);
      break;
  }
}
void all_motor_control(uint32_t A_angle, uint32_t B_angle, uint32_t C_angle){
  motor_control(MCPWM_GPIO_A,A_angle);
  motor_control(MCPWM_GPIO_B,B_angle);
  motor_control(MCPWM_GPIO_C,C_angle);
}
void mc_pwm_config(){
  printf("initializing mcpwm servo control gpio......\n");
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, MCPWM_GPIO_A);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, MCPWM_GPIO_B);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, MCPWM_GPIO_C);
  printf("Configuring Initial Parameters of mcpwm......\n");
  mcpwm_config_t pwm_config;
  pwm_config.frequency = 50;    //frequency = 50Hz, i.e. for every servo motor time period should be 20ms
  pwm_config.cmpr_a = 0;    //duty cycle of PWMxA = 0
  pwm_config.cmpr_b = 0;    //duty cycle of PWMxb = 0
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);    //Configure PWM0A & PWM0B with above settings
}

uint32_t servo_per_degree_init(uint32_t degree_of_rotation){
    uint32_t cal_pulsewidth = 0;
    cal_pulsewidth = (SERVO_MIN_PULSEWIDTH + (((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * (degree_of_rotation)) / (SERVO_MAX_DEGREE)));
    return cal_pulsewidth;
}
void mcpwm_example_servo_control(void *arg){
    uint32_t angle, count;
    mc_pwm_config();
    while (1) {
      for (count = 0; count < SERVO_MAX_DEGREE; count++) {
        printf("Angle of rotation: %d\n", count);
        angle = servo_per_degree_init(count);
        printf("pulse width: %dus\n", angle);
        mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, angle);
        mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 1500);
        vTaskDelay(10);     //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation at 5V
      }
    }
}

void servo_control_task(void* arg){
  mc_pwm_config();
  float p = 0.015;
  float i = 0.0001;
  float d = 0.15;
  int8_t motor_deg[3] = {0};
  uint8_t now_flag = 0;
  uint8_t last_flag = 0;
  int8_t default_angle = 45;
  Coordinate now_e;
  Coordinate last_e;
  while(1){
    now_flag = check_ctrl_flag(Pilko);
    if(now_flag == true){
      Coordinate now_coord = Pilko.coord;
      Coordinate error_coord = {
        .x = target_coord.x - now_coord.x,
        .y = target_coord.y - now_coord.y
      };
      if((fabs(error_coord.x - last_e.x) >= 100 || fabs(error_coord.x - last_e.x) >= 100) 
          && last_flag == true){
        now_e = last_e;
        printf("Fixed!!!!!!!!\n");
      }
      else now_e = error_coord;
      printf("error_coord is x %d, y %d\n", error_coord.x, error_coord.y);
      //error_coord.x -= error_coord.y * sqrt(3); // Fix vector
      motor_deg[0] = (p * -now_e.x + d *((-now_e.x) - (-last_e.x))) * sqrt(3);
      motor_deg[1] = p * -now_e.y + d *((-now_e.y) - (-last_e.y)) + (p * now_e.x);
      motor_deg[2] = p * now_e.y + d *(now_e.y - last_e.y) + (p * now_e.x);
      //motor_deg[2] = p * now_e.y;
      /*
      if((error_coord.x >= 0 && error_coord.y >= 0) || (error_coord.x <= 0 && error_coord.y <= 0)){
        motor_deg[2] = p * error_coord.y;
        motor_deg[1] = 0;
      }
      else{
        motor_deg[2] = 0;
        motor_deg[1] = p * -error_coord.y;
      }
      */
      //motor_deg[0] = p * -error_coord.x;
      printf("motor_deg0 %d motor_deg1 %d  motor_deg2 %d \n", motor_deg[0], motor_deg[1], motor_deg[2]);
    }
    last_flag = now_flag;

    all_motor_control(default_angle + motor_deg[0], default_angle+ motor_deg[1], default_angle + motor_deg[2]);
    last_e = now_e;

    vTaskDelay(5);
  }
}

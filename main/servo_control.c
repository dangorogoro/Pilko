#include "mine.h"

#define SERVO_MIN_PULSEWIDTH 600 //Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH 2400 //Maximum pulse width in microsecond
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
}

uint32_t servo_per_degree_init(uint32_t degree_of_rotation){
    uint32_t cal_pulsewidth = 0;
    cal_pulsewidth = (SERVO_MIN_PULSEWIDTH + (((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * (degree_of_rotation)) / (SERVO_MAX_DEGREE)));
    return cal_pulsewidth;
}

void mcpwm_example_servo_control(void *arg){
    uint32_t angle, count;
    mc_pwm_config();

    printf("Configuring Initial Parameters of mcpwm......\n");
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 50;    //frequency = 50Hz, i.e. for every servo motor time period should be 20ms
    pwm_config.cmpr_a = 0;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);    //Configure PWM0A & PWM0B with above settings
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


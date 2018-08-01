#ifndef MY_CONFIG_H
#define MY_CONFIG_H
typedef struct{
  Coordinate coord;
  MotorUnit servo[3];
//  Led charlie;
}Controller;
//  Led charlie;
extern Controller Pilko;
uint8_t check_ctrl_flag(Controller po);
#endif

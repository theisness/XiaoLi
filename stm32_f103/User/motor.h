#ifndef __MOTOR_H_
#define __MOTOR_H_
#include "lib_common.h"

void MotorSetPwm(int16_t pwm1, int16_t pwm2);
void MotorStop(uint8_t op);
void MotorInit(void);
#endif

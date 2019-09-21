#ifndef PCA9685_H
#define PCA9685_H
/*
  这个驱动是在树莓派的wiringPi基础上的，基于wiringPi对i2c的接口函数。
  此驱动的使用方法是：
  1、先用 pca9685_init(从机地址) 初始化，得到一个设备描述符（int型），这个设备描述符代表这个pca9685芯片，因为可能多个pca9685级联，通过这个设备描述符来区分它们。
    它的
  2、调用 pca9685_setmk
 */
#include "lib_common.h"


#define PCA9685_adrr 0x80
#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE


#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

void PCA9685_write(unsigned char reg,unsigned char data);
u8 PCA9685_read(unsigned char reg);
void setPWMFreq(u8 freq);
void setPWM(u8 num, u16 on, u16 off);
void down(void);
void up(void);
u16 calculate_PWM(u8 angle);
#endif


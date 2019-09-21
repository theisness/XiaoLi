#ifndef __SERVO_H_
#define __SERVO_H_

#include "lib_common.h"
#include "lib_math.h"
#include "pca9685.h"

void ServoInit(void);
void ServoStart(void);
void ServoTest(void);
void ServoDirCtl(int8_t* sctl );
void SetServoDefault(void);
void _SetServoPulseValue(u8 num, u16 pulse_width);
float _SetServoAngle(uint8 channel, float angle);
float SetServoAngle(uint8 channel, float angle);
float SetServoRad(uint8 channel, float rad);
void ServoStart(void);
void SetServoStop(void);
float GetServoAngle(u8 channel);
#define ServoOff() PBout(3) = 0
#define ServoOn() PBout(3) = 1

#define SERVO_NUM 6
extern float SERVO_ANGLE_SCALE[SERVO_NUM][3];
extern const float SERVO_DEFAULT_ANGLE[SERVO_NUM];
#endif

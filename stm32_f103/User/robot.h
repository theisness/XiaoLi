#ifndef __ROBOT_H_
#define __ROBOT_H_


#include "lib_common.h"
#include "lib_math.h"
#include "servo.h"
#include "motor.h"
#include "ps2.h"

#define CAM_ON() PAout(11)=0
#define CAM_OFF() PAout(11)=1
void RobotInit(void);
void MotorGo(void);
void ServoGo(void);
void RobotTest(void);
void CamDataCallBack(uint8* buf);
void SetFollow(void);
void StopFollow(void);
void Follow(void);
void AutoGo(void);
#endif 

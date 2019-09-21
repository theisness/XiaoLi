#include "servo.h"
#include "pca9685.h"
#include "ps2.h"
#include "motor.h"
#include "lib_math.h"

const u8 SERVO_DEBUG = 0;
#define s0min 102
#define s0max 512

#define s1min 102
#define s1max 512

#define s2min 102
#define s2max 512

#define s3min 102
#define s3max 512

#define s4min 102
#define s4max 512

#define s5min 246
#define s5max 480
const int16 _SERVO_PULSE_SCALE[SERVO_NUM][3] ={
 {s0min, s0max-s0min, s0max},
 {s1min, s1max-s1min, s1max},
 {s2min, s2max-s2min, s2max},
 {s3min, s3max-s3min, s3max},
 {s4min, s4max-s4min, s4max},
 {s5min, s5max-s5min, s5max},
};
float SERVO_ANGLE_SCALE[SERVO_NUM][3] = {
 {0, 180, 0},
 {0, 180, 0},
 {0, 180, 0},
 {0, 180, 0},
 {0, 180, 0},
 {0, 90,  0},
};
const float SERVO_DEFAULT_ANGLE[SERVO_NUM] ={
	90, 90, -0, -90, 90, 8
};
/*
robot overlook:
		Y
	  |					^ y
	O---O				|
	O---O				.--->x
right elevation:
	|-->          ^ theta
O---O						|
*/
//This is the angle of offset between destination angle and original angle in Spherical Coordinates.
const float SERVO_OFFSET_ANGLE[SERVO_NUM] = {  
	10,	-6,	-103,	-40,	0,	0
};
//This is the servo rotating direction. The same with Spherical Coordinates = 1.
const float SERVO_ROTATE_DIR[SERVO_NUM] = {
	1,	1,	-1,		-1,		1,	1
};
float servo[SERVO_NUM] = {0};

void ServoInit(){//初始化驱动板
	PCA9685_write(PCA9685_MODE1,0);
	setPWMFreq(50);//输出PWM频率为50HZ
	for(u8 i=0; i<SERVO_NUM; i++){
		if( SERVO_ROTATE_DIR[i] == 1){
			SERVO_ANGLE_SCALE[i][0] = 0 - SERVO_OFFSET_ANGLE[i];
			SERVO_ANGLE_SCALE[i][2] = SERVO_ANGLE_SCALE[i][0] + SERVO_ANGLE_SCALE[i][1];
		}
		else{
			SERVO_ANGLE_SCALE[i][0] = -R2A(PI) - SERVO_OFFSET_ANGLE[i];
			SERVO_ANGLE_SCALE[i][2] = SERVO_ANGLE_SCALE[i][0] + SERVO_ANGLE_SCALE[i][1];
		}
		
	}
}
void ServoStart(){
	for(u8 i=0; i<SERVO_NUM; i++){
	  servo[i] = SERVO_DEFAULT_ANGLE[i];
		SetServoAngle(i, servo[i]);
	}
}
void _SetServoPulseValue(u8 num, u16 pulse_width){
    PCA9685_write(LED0_ON_L+4*num,0);
    PCA9685_write(LED0_ON_H+4*num,0>>8);
    PCA9685_write(LED0_OFF_L+4*num,pulse_width);
    PCA9685_write(LED0_OFF_H+4*num,pulse_width>>8);
}
void SetServoStop(){
	for(uint8 i=0; i <SERVO_NUM; i++){
		_SetServoPulseValue(i, 0);
	}
}
void SetServoDefault(){
	for(u8 i=0; i<SERVO_NUM; i++){
       SetServoAngle(i, SERVO_DEFAULT_ANGLE[i]);
   }
}

float _SetServoAngle(uint8 channel, float angle){
	int16 pulse = _SERVO_PULSE_SCALE[channel][1]
		* angle/SERVO_ANGLE_SCALE[channel][1] + _SERVO_PULSE_SCALE[channel][0];
	pulse = LIMIT(pulse, _SERVO_PULSE_SCALE[channel][0], _SERVO_PULSE_SCALE[channel][2]); 
	_SetServoPulseValue(channel, pulse);
	angle = (pulse - _SERVO_PULSE_SCALE[channel][0])*SERVO_ANGLE_SCALE[channel][1]/_SERVO_PULSE_SCALE[channel][1] + 0;
	return angle;
}

float SetServoAngle(uint8 channel, float angle){
	float angle_sph = (angle + SERVO_OFFSET_ANGLE[channel])*SERVO_ROTATE_DIR[channel];
	angle_sph = _SetServoAngle(channel, angle_sph);
	angle = angle_sph*SERVO_ROTATE_DIR[channel] - SERVO_OFFSET_ANGLE[channel];
	servo[channel] = angle;
	return angle;
}

float SetServoRad(uint8 channel, float rad){
	float ag = SetServoAngle(channel, R2A(rad));
	return A2R(ag);
}
float GetServoAngle(u8 channel){
	return servo[LIMIT(channel, 0, SERVO_NUM-1)];
}


//********************The code for test***********************//
void ServoTest(){
    up();
    delay_ms(500);
    down();
    delay_ms(500);
}
#define res 1
 void ServoDirCtl(int8_t sctl[SERVO_NUM] ){
    for(u8 i=0; i<SERVO_NUM; i++){
        servo[i] += res*sctl[i];
    }
   for(u8 i=0; i<SERVO_NUM; i++){
       SetServoAngle(i, servo[i]);
		 if(SERVO_DEBUG)
			printf("sv%d= %.2f ", i, servo[i]);
   }
	 if(SERVO_DEBUG) printf("\r\n");
}


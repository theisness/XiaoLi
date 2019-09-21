#include "motor.h"
#include "lib_common.h"
#include "lib_math.h"
#include "tim.h"


#define MAX_CCR 4095

#define IN1H PBout(1)=1
#define IN1L PBout(1)=0
#define IN2H PBout(0)=1
#define IN2L PBout(0)=0
#define CH1_DIR2 IN1H; IN2L
#define CH1_DIR1 IN1L; IN2H
#define CH1_HANG IN1H; IN2H
#define CH1_STOP IN1L; IN2L

//#define IN3H P3OUT |= BIT0
//#define IN3L P3OUT &= ~BIT0
#define IN3H PAout(7)=1
#define IN3L PAout(7)=0
#define IN4H PAout(6)=1
#define IN4L PAout(6)=0
#define CH2_DIR2 IN3H; IN4L
#define CH2_DIR1 IN3L; IN4H
#define CH2_HANG IN3H; IN4H
#define CH2_STOP IN3L; IN4L

#define MOTOR_TIMER htim2
extern TIM_HandleTypeDef MOTOR_TIMER;
#define CCR1 TIM2->CCR4
#define CCR2 TIM2->CCR3

void MotorInit(){
   HAL_TIM_PWM_Start(&MOTOR_TIMER, TIM_CHANNEL_3);
   HAL_TIM_PWM_Start(&MOTOR_TIMER, TIM_CHANNEL_4);
	
}
void MotorSetPwm(int16_t pwm1, int16_t pwm2){
    if(pwm1 > 0){
        CH1_DIR1;
        CCR1 = LIMIT(pwm1, 0, MAX_CCR);
    }
    else if(pwm1 < 0){
        CH1_DIR2;
        pwm1= -pwm1;
        CCR1 = LIMIT(pwm1, 0, MAX_CCR);
    }
    else{
        CH1_HANG;
        CCR1 = 0;
    }
    if(pwm2 > 0){
        CH2_DIR1;
        CCR2 = LIMIT(pwm2, 0, MAX_CCR);
    }
    else if(pwm2 < 0){
        CH2_DIR2;
        pwm2= -pwm2;
        CCR2 = LIMIT(pwm2, 0, MAX_CCR);
    }
    else {
        CH2_HANG;
        CCR2 = 0;
    }
}

void MotorStop(uint8_t op){
    switch(op){
    case 1:
      CH1_STOP;
      CCR1 = 0;
    break;
    case 2:
      CH2_STOP;
      CCR2 = 0;
    break;
    case 3:
       CH1_STOP; CH2_STOP;
       CCR1 = 0; CCR2 = 0;
    }
}

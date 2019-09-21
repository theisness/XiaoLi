#include "motor.h"
#include "ps2.h"
#include "servo.h"
#include "robo_arm.h"
#include "usart.h"
#include "robot.h"

#define P_TURN 16
#define P_STRAIGHT 22

void RobotInit(){
	RoboArmInit();
	MotorInit();
	ServoStart();
	SetUARTCallBack(CamDataCallBack);
}
void CarGo(){
    int16_t y = PS2_GetLY();
    int16_t x = PS2_GetLX();
		if( y<0)x = -x;
    int16_t pwm1 = x*P_TURN + y*P_STRAIGHT;
    int16_t pwm2 = -x*P_TURN + y*P_STRAIGHT;
    MotorSetPwm(pwm1, pwm2);
}

void ServoGo(){
    int16_t x = PS2_GetRX();
    int16_t y = PS2_GetRY();
    int8_t svo[6]={0};
    if(x >20) svo[0]--;
    else if(x<-20)svo[0] ++;
    else  svo[0] = 0;
    if(y>20){svo[1]--; svo[2]++;}
    else if(y<-20){svo[1]++; svo[2]--;}
    else {svo[1]=0; svo[2]=0;}

    if(IsPushed(BTN_L1)){svo[2]++;}
    else if(IsPushed(BTN_L2)){svo[2]--;}
    if(IsPushed(BTN_R1)){svo[3]++;}
    else if(IsPushed(BTN_R2)){svo[3]--;}

    if(IsPushed(BTN_PINK)){svo[4]--;}
    else if(IsPushed(BTN_RED)){svo[4]++;}
    if(IsPushed(BTN_GREEN)){svo[5]--;}
    else if(IsPushed(BTN_BLUE)){svo[5]++;}
    ServoDirCtl(svo);
	
}
void RoboArmGo_sph(){
	  int16_t x = PS2_GetRX();
    int16_t y = PS2_GetRY();
    sph_vect motion_vec = {0};
    if(x >20) motion_vec.phi-=A2R(1.5);
    else if(x<-20)motion_vec.phi+=A2R(1.5);
    else  motion_vec.phi=0;
    if(y>20) motion_vec.theta+=A2R(1.2);
    else if(y<-20) motion_vec.theta-=A2R(1.2);
    else motion_vec.theta = 0;

    if(IsPushed(BTN_R1)) motion_vec.r += 0.5;
    else if(IsPushed(BTN_R2)) motion_vec.r -= 0.5;

		SetArmMotion_sph_rel(&motion_vec);
		
		int8_t svo[6]={0};
    if(IsPushed(BTN_PINK)){svo[4]-=2;}
    else if(IsPushed(BTN_RED)){svo[4]+=2;}
    if(IsPushed(BTN_GREEN)){svo[5]-=2;}
    else if(IsPushed(BTN_BLUE)){svo[5]+=2;}
    ServoDirCtl(svo);
}
void RoboArmGo_card(){
	  int16_t x = PS2_GetRX();
    int16_t y = PS2_GetRY();
		const u8 thresh = 120;
    card3_vect motion_vec = {0, 0, 0};
    if(x >thresh) motion_vec.x+=1;
    else if(x<-thresh)motion_vec.x-=1;
    else  motion_vec.x=0;
    if(y>thresh) motion_vec.y+=1;
    else if(y<-thresh) motion_vec.y-=1;
    else motion_vec.y = 0;

    if(IsPushed(BTN_R1)) motion_vec.z += 1;
    else if(IsPushed(BTN_R2)) motion_vec.z -= 1;

		SetArmMotion_card_abs(&motion_vec);
}
void RobotTest(){
	PS2_Update();
	if(IsPS2_OK()){
		CarGo();
		//ServoGo();
		AutoGo();
	}
	else{
		MotorStop(3);
		SetServoStop();
	}
}
enum {mannul, autogo}MODE;
void AutoGo(){
	static uint8 state = mannul;
	if(IsPushed(BTN_START)){
		state = autogo;
	}
	else if(IsPushed(BTN_SELECT)){
		state = mannul;
	}
	if(state==mannul){
		StopFollow();
		RoboArmGo_sph();
	}
	else if(state==autogo){
		Follow();
	}
	
}
#define CENTERX 80
#define CENTERY 60

int8 auto_flag = 0;
int8 get_target = 0;
int16 target_x=0;
int16 target_y=0;
int16 radius = 0;+
uint8 clamp = 0;
#define CLAMP_NUM 1000
#define CLAMP_CENTERX 80
#define CLAMP_CENTERY 93.5
#define STANDARD_RADIUS 20


#define  Clamp() SetServoAngle(5, 40)
#define DeClamp() SetServoAngle(5, SERVO_DEFAULT_ANGLE[5])
typedef enum{clamping, declamping, clamped}CLAMP_STATE;
void Follow(){
	static float px=0.0015, py=0.05, pz=0.0015;
	static int8 state = declamping;
	float cx,cy, cz;
	SetFollow();
	if(get_target){
		int16 disx, disy, disr;
		
		if(state == clamped){
			/*sph_vect v={A2R(90), A2R(70), 25};
			SetArmPos_sph( &v);*/
			if(IsPushed(BTN_START)){
				state = declamping;
				DeClamp();
			}
			return;
		}
		else if(state==clamping){
			if(IsPushed(BTN_GREEN)){
				state = declamping;
				DeClamp();
				return;
			}
			cx = CLAMP_CENTERX;
			cy = CLAMP_CENTERY;
			disx = cx-target_x;
			disy = cy-target_y;
			disr = STANDARD_RADIUS-radius;
			if(ABS(disx) <2 &&ABS(disy)<2 && ABS(disr)<2){
				Clamp();
				for(int i=0; i<200; i++){
					PS2_Update();
					if(IsPushed(BTN_START)){
						state = declamping;
						return;
					}
					delay_ms(10);
				}
				sph_vect v = { PI/2, A2R(70), 25};
				SetArmPos_sph(&v);
				state=clamped;
			}
			py = 0.05;
		}
		else if(state==declamping){
			DeClamp();
			cx = CENTERX;
			cy = CENTERY;
			disx = cx-target_x;
			disy = cy-target_y;
			if(ABS(disx) <2 &&ABS(disy)<2 ){
				state = clamping;
			}
			py=0;
		}
		
		
		
		sph_vect mv;
		mv.phi = disx*px;
		mv.theta = disy*pz;
		mv.r = disr*py;
		printf("x=%.2f y=%.2f r=%d\r\n", mv.phi, mv.theta, radius);
		//mv.r = 0;
		SetArmMotion_sph_rel(&mv);
		get_target = 0;
	}
	
}
void SetFollow(){
	CAM_ON();
	auto_flag=1;
}
void StopFollow(){
	CAM_OFF();
	auto_flag =0;
	get_target=0;
}

void CamDataCallBack(uint8* buf){
	const u8 pos[4] = {0,4,8,12};
	uint16 x, y, r;
	if(auto_flag==0 )return;
	//printf((char*)buf);
	if(buf[pos[0]]=='x'&&buf[pos[0]+1]=='='){
		x = *(uint16*)(buf + pos[0]+2);
	}
	else return;
	if(buf[pos[1]]=='y'&&buf[pos[1]+1]=='='){
		y = *(uint16*)(buf + pos[1]+2);
	}
	else return;
	if(buf[pos[2]]=='r'&&buf[pos[2]+1]=='='){
		r = *(uint16*)(buf + pos[2]+2);
	}
	else return;
	target_x = x; target_y = y; radius =r; get_target =1;
	
}


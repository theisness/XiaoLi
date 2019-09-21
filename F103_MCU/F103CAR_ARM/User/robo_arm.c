#include "robo_arm.h"
#include "lib_math.h"
#include  "math.h"

#define JOINT_NUM 3
#define ROD_NUM 3

const float ROD_LEN[ROD_NUM] = {10.5, 10, 20}; //in celimeter
float MAX_JOINT1_LEN = 0;
float MIN_JOINT1_LEN = 0;
float MAX_JOINT2_LEN = 0;

polor_vect joint[JOINT_NUM];
polor_vect rod[ROD_NUM];

float rotate_phi;
float claw_rot_rad;
float clamp_rad;

float LIMIT_ROD_RAD[ROD_NUM][2];
float LIMIT_ROTATE_PHI[2];


//6 paras are needed: 3 rods vectors, rotate_phi, claw_rot, clamp.
void RoboArmUpdateState(){
	
	rod[0].theta = LIMIT(rod[0].theta, LIMIT_ROD_RAD[0][0], LIMIT_ROD_RAD[0][1]);
	rod[1].theta = LIMIT(rod[1].theta-rod[0].theta, LIMIT_ROD_RAD[1][0], LIMIT_ROD_RAD[1][1]) + rod[0].theta;
	rod[2].theta = LIMIT(rod[2].theta-rod[1].theta, LIMIT_ROD_RAD[2][0], LIMIT_ROD_RAD[2][1]) + rod[1].theta;
	
	rotate_phi = LIMIT(rotate_phi, LIMIT_ROTATE_PHI[0], LIMIT_ROTATE_PHI[1]);
	SetServoRad(0, rotate_phi);  //0
	SetServoRad(1, rod[0].theta);							//1
	SetServoRad(2, rod[1].theta - rod[0].theta);//2
	SetServoRad(3, rod[2].theta - rod[1].theta);//3
	/*claw_rot_rad = SetServoRad(4, claw_rot_rad);//4
	clamp_rad = SetServoRad(5, clamp_rad);//5*/

	joint[0].theta = rod[0].theta;
	joint[0].rho = rod[0].rho;
	polor_vadd(&joint[1], &joint[0], &rod[1]);
	polor_vadd(&joint[2], &joint[1], &rod[2]);
}
void RoboArmInit(){
	ServoInit();
	ServoStart();
	LIMIT_ROTATE_PHI[0] = A2R(SERVO_ANGLE_SCALE[0][0]);
	LIMIT_ROTATE_PHI[1] = A2R(SERVO_ANGLE_SCALE[0][1]);
	LIMIT_ROD_RAD[0][0] = A2R(SERVO_ANGLE_SCALE[1][0]);
	LIMIT_ROD_RAD[0][1] = A2R(SERVO_ANGLE_SCALE[1][2]);
	LIMIT_ROD_RAD[1][0] = A2R(SERVO_ANGLE_SCALE[2][0]);
	LIMIT_ROD_RAD[1][1] = A2R(SERVO_ANGLE_SCALE[2][2]);
	LIMIT_ROD_RAD[2][0] = A2R(SERVO_ANGLE_SCALE[3][0]);
	LIMIT_ROD_RAD[2][1] = A2R(SERVO_ANGLE_SCALE[3][2]);
	for(u8 i=0; i<ROD_NUM; i++){
		rod[i].rho = ROD_LEN[i];
		rod[i].theta = A2R(GetServoAngle(i+1));
		MAX_JOINT2_LEN += ROD_LEN[i];
	}
	MAX_JOINT1_LEN = ROD_LEN[0] + ROD_LEN[1];
	double minj1_1 = CosLaw_SolSide(ROD_LEN[0], ROD_LEN[1], PI -LIMIT_ROD_RAD[1][0]);
	double minj1_2 = CosLaw_SolSide(ROD_LEN[0], ROD_LEN[1], PI -LIMIT_ROD_RAD[1][1]);
	MIN_JOINT1_LEN = min(minj1_1, minj1_2);
	
	rotate_phi = A2R(GetServoAngle(0));
	claw_rot_rad = A2R(GetServoAngle(4));
	clamp_rad = A2R(GetServoAngle(5));
	rod[1].theta += rod[0].theta;
	rod[2].theta += rod[1].theta;
	joint[0].theta = rod[0].theta;
	joint[0].rho = rod[0].rho;
	polor_vadd(&joint[1], &joint[0], &rod[1]);
	polor_vadd(&joint[2], &joint[1], &rod[2]);
	
}
// to see is rod[3]'s rad applicable
uint8 ApplicateRod(double*rad0, double*rad1, double* rad2){
	double a0 = *rad0;	
	double a2 = *rad2 - *rad1;
	double a1 = *rad1 - *rad0;

	if(IS_INSCALE(a0, LIMIT_ROD_RAD[0][0], LIMIT_ROD_RAD[0][1]) &&
			IS_INSCALE(a1, LIMIT_ROD_RAD[1][0], LIMIT_ROD_RAD[1][1]) &&
			IS_INSCALE(a2, LIMIT_ROD_RAD[2][0], LIMIT_ROD_RAD[2][1]) )
	{
		rod[0].theta = *rad0;
		rod[1].theta = *rad1;
		rod[2].theta = *rad2;
		return 1;
	}
	return 0;
}
//when limit=1, would'not change rod[2]
//when limit=0, will change it.
uint8 ApplicateJoint2(polor_vect*j2){
	polor_vect tmp_joint1, tmp_joint2 = *j2, tmp_rod2 = rod[2];
	if(tmp_joint2.rho>MAX_JOINT2_LEN){
		// Though unsolvable, but reach out to straight.
		rod[0].theta = rod[1].theta = rod[2].theta = tmp_joint2.theta;
		return 1;
	}
	else{
		polor_vsub(&tmp_joint1, &tmp_joint2, &tmp_rod2);
		//the joint1 reach out to straight, resolve.
		if(tmp_joint1.rho>MAX_JOINT1_LEN){
			tmp_joint1.rho = MAX_JOINT1_LEN;
			TriVect_SolAng(&tmp_joint1, &tmp_rod2, &tmp_joint2, 1);
			if(ApplicateRod(&tmp_joint1.theta, &tmp_joint1.theta, &tmp_rod2.theta)){
					return 1;
			}
			else{
				//resolve the triangle
				TriVect_SolAng(&tmp_joint1, &tmp_rod2, &tmp_joint2, -1);
				if(ApplicateRod(&tmp_joint1.theta, &tmp_joint1.theta, &tmp_rod2.theta)){
					return 1;
				}
				else{
					//UNSOLVABLE!!! return 0.
					return 0;
				}
			}
		}
		else{
			if(ApplicateJoint1(&tmp_joint1, &tmp_rod2)){ 
				return 1;
			}
			else{				
				//still unapplicable, so resolve the first equation
				tmp_joint1.rho = limit(tmp_joint1.rho +1, MIN_JOINT1_LEN, min(tmp_joint2.rho, MAX_JOINT1_LEN));
				TriVect_SolAng(&tmp_joint1, &tmp_rod2, &tmp_joint2, 1);
				if( ApplicateJoint1(&tmp_joint1, &tmp_rod2)){ 
					return 1;
				}
				else{
					TriVect_SolAng(&tmp_joint1, &tmp_rod2, &tmp_joint2, -1);
					if( ApplicateJoint1(&tmp_joint1, &tmp_rod2)){ 
						return 1;
					}
					else{
						//UNSOLVABLE!!!
						return 0;
					}
				}
			}
		}
	}
}
uint8 ApplicateJoint1(polor_vect*j1, polor_vect*r2){
	polor_vect tmp_rod0 = rod[0], tmp_rod1 = rod[1];
	TriVect_SolAng( &tmp_rod0, &tmp_rod1, j1, 1);
	if( ApplicateRod(&tmp_rod0.theta, &tmp_rod1.theta, &r2->theta)){
		return 1;
	}
	else{
		
		/*TriVect_SolAng( &tmp_rod0, &tmp_rod1, j1, -1);
		if( ApplicateRod(&tmp_rod0.theta, &tmp_rod1.theta, &r2->theta)){
			return 1;
		}
		else{
			return 0;
		}*/
		return 0;
	}
}
void SetArmPos_sph(sph_vect* sv){
	polor_vect tmp_joint2 = {sv->theta, sv->r};
	if( ApplicateJoint2(&tmp_joint2)){
		rotate_phi = sv->phi;
		RoboArmUpdateState();
	}
}
// The vect->r present the direction as forword or backword while
// vect->theta and phi present the dirction of WASD in a relative perspect.
//	please notes that the Vector here just present the motion direction which
//	is not the same as that in the card coodinates!!
void SetArmMotion_sph_rel(sph_vect* mvec){	
	polor_vect tmp_joint2 = joint[1];
	rod[2].theta += mvec->theta;
	polor_vadd(&tmp_joint2, &tmp_joint2, &rod[2]);
	polor_vect p_mvec = {rod[2].theta, mvec->r};
	polor_vadd(&tmp_joint2, &tmp_joint2, &p_mvec);
	sph_vect sv = {mvec->phi + rotate_phi, tmp_joint2.theta, tmp_joint2.rho};
	SetArmPos_sph(&sv);
}
// The vect->r present the direction as forword or backword while
// vect->theta and phi present the dirction of WASD in a absolute perspect.
//	please notes that the Vector here just present the motion direction which
//	is not the same as that in the card coodinates!!
void SetArmMotion_sph_abs(sph_vect* mvec){
	polor_vect tmp_joint2 = joint[2];
	tmp_joint2.theta += mvec->theta; tmp_joint2.rho += mvec->r;
	sph_vect sv = {rotate_phi+mvec->phi, tmp_joint2.theta, tmp_joint2.rho};
	SetArmPos_sph(&sv);
}
//In Cartesian coordinates
void SetArmMotion_card_abs(card3_vect* mvect){
	sph_vect sj2 = {rotate_phi, joint[2].theta, joint[2].rho}, dsj2;
	card3_vect cj2, dcj2;
	SphtCard(&sj2, &cj2);
	card3_vadd(&dcj2, &cj2, mvect);
	CardtSph(&dcj2, &dsj2);
	sph_vect s_mvect;
	
	//please notes that the Vector subtraction here 
	//is not the same as that in the card coodinates!!
	s_mvect.phi = dsj2.phi - sj2.phi;
	s_mvect.theta = dsj2.theta - sj2.theta;
	s_mvect.r = dsj2.r - sj2.r;
	
	SetArmMotion_sph_abs(&s_mvect);
	/*sph_vect sj = {rotate_phi, joint[2].theta, joint[2].rho};
	card3_vect cj;
	SphtCard(&sj, &cj);
	printf("x=%.2f, y=%.2f, z=%.2f\r\n", cj.x, cj.y, cj.z);*/
}
void SetArmMotion_card_rel(card3_vect* mvect){
	card3_vect m_vect;
	sph_vect rod2 = {rotate_phi, rod[2].theta, rod[2].rho};
	card3_rot_csys(&m_vect, mvect, -rod2.theta, 0, -(rod2.phi-PI/2), "xz");
	
	SetArmMotion_card_abs(&m_vect);
}

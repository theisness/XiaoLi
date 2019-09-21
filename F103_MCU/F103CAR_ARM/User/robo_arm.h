#ifndef ROBO_ARM_H
#define ROBO_ARM_H

#include "lib_common.h"
#include "servo.h"

void RoboArmInit(void);
void SetArmPos_sph(sph_vect* sv);
void SetArmMotion_sph_rel(sph_vect* mvec);
void SetArmMotion_sph_abs(sph_vect* mvec);
void SetArmMotion_card_rel(card3_vect* mvect);
void SetArmMotion_card_abs(card3_vect* mvect);
uint8 ApplicateJoint1(polor_vect*j1, polor_vect*r2);
#endif

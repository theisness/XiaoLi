#ifndef __PS2_H_
#define __PS2_H_

#include "lib_common.h"
void PS2_SetInit(void);
void PS2_Init(void);
void PS2_Update(void);
void PS2_Test(void);
void PS2_Vibration(u8 motor1, u8 motor2);
int8_t PS2_GetLX(void);
int8_t PS2_GetRX(void);
int8_t PS2_GetLY(void);
int8_t PS2_GetRY(void);
u8 IsPushed(u8 key);
u8 IsPS2_OK(void);

#define DATA_BYTES 9
#define KEY_NUM 16

#define BTN_SELECT      1
#define BTN_L3          2
#define BTN_BTN_R3          3
#define BTN_START       4
#define BTN_UP      5
#define BTN_RIGHT   6
#define BTN_DOWN    7
#define BTN_LEFT    8
#define BTN_L2         9
#define BTN_R2          10
#define BTN_L1          11
#define BTN_R1          12
#define BTN_GREEN       13
#define BTN_RED         14
#define BTN_BLUE        15
#define BTN_PINK        16

extern const u8 KeyName[KEY_NUM][10];
extern u8 PS2_Data[DATA_BYTES];
#endif

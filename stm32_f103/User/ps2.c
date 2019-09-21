#include "ps2.h"
#include "lib_common.h"
#include "stdio.h"


u8 PS2_Data[DATA_BYTES];
const u8 KeyName[KEY_NUM][10]={
	"SELECT",
	"L3",
	"R3",
	"START",
	"UP",
	"RIGHT",
	"DOWN",
	"LEFT",
	"L2",
	"R2",
	"L1",
	"R1",
	"GREEN",
	"RED",
	"BLUE",
	"PINK"
};
#define DI  PBin(6)
#define DO(x) PBout(7)=(x)
#define CS(x) PBout(8)=(x)
#define CLK(x) PBout(9)=(x)
 
#define PS2_DELAY delay_us(16)
void PS2_Init(){
	   //输入  DI->PB12
	/*RCC->APB2ENR|=1<<3;     //使能PORTB时钟
	GPIOB->CRH&=0XFFF0FFFF;//PB12设置成输入	默认下拉  
	GPIOB->CRH|=0X00080000;   

    //  DO->PB13    CS->PB14  CLK->PB15
	RCC->APB2ENR|=1<<3;    //使能PORTB时钟  	   	  	 
	GPIOB->CRH&=0X000FFFFF; 
	GPIOB->CRH|=0X33300000;//PB13、PB14、PB15 推挽输出 */
	PS2_SetInit();
}
static void PS2_Cmd(u8 CMD){
	volatile u16 ref=0x01;
	PS2_Data[1] = 0;
	for(ref=0x01;ref<0x0100;ref<<=1){
		if(ref&CMD){
			DO(1);                   //输出以为控制位
		}
		else DO(0);

		CLK(1);                        //时钟拉高
		PS2_DELAY;
		CLK(0);
		PS2_DELAY;
		CLK(1);
		if(DI)
			PS2_Data[1] = ref|PS2_Data[1];
	}
}
static void PS2_ShortPoll(){
	CS(0);
	PS2_DELAY;
	PS2_Cmd(0X1);
	PS2_Cmd(0X42);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	CS(1);
	PS2_DELAY;
}
static void PS2_EnterConfig(){
	CS(0);
	PS2_DELAY;
	PS2_Cmd(0x01);
	PS2_Cmd(0x43);
	PS2_Cmd(0x00);
	PS2_Cmd(0x01);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	CS(1);
	PS2_DELAY;
}
static void PS2_TurnOnAnalogMode(){
	CS(0);
	PS2_Cmd(0x01);
	PS2_Cmd(0x44);
	PS2_Cmd(0x00);
	PS2_Cmd(0x01);
	PS2_Cmd(0x0ee);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	CS(1);
	PS2_DELAY;
}
static void PS2_VibrationMode(){
	CS(0);
	PS2_DELAY;
	PS2_Cmd(0x01);
	PS2_Cmd(0x4d);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x01);
	CS(1);
	PS2_DELAY;
}
static void PS2_ExitConfig(){
	CS(0);
	PS2_DELAY;
	PS2_Cmd(0x01);
	PS2_Cmd(0x43);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x5a);
	PS2_Cmd(0x5a);
	PS2_Cmd(0x5a);
	PS2_Cmd(0x5a);
	PS2_Cmd(0x5a);
	CS(1);
	PS2_DELAY;
}
static void PS2_SetInit(){
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_EnterConfig();
	PS2_TurnOnAnalogMode();
	PS2_VibrationMode();
	PS2_ExitConfig();
}
u8 PS2_RedLight(void){
	CS(0);
	PS2_Cmd(0x01);  //开始命令
	PS2_Cmd(0x42);  //请求数据
	CS(1);
	if( PS2_Data[1] == 0X73)   return 0 ;
	else return 1;
}
void PS2_Vibration(u8 motor1, u8 motor2){
	CS(0);
	PS2_DELAY;
	PS2_Cmd(0x01);
	PS2_Cmd(0x42);
	PS2_Cmd(0x00);
	PS2_Cmd(motor1);
	PS2_Cmd(motor2);
	PS2_Cmd(0);
	PS2_Cmd(0);
	PS2_Cmd(0);
	PS2_Cmd(0);
	CS(1);
	PS2_DELAY;
}
void PS2_ReadData(void){
	volatile u8 byte=0;
	volatile u16 ref=0x01;
	CS(0);
	PS2_Cmd(0X01);  //开始命令
	PS2_Cmd(0X42);  //请求数据
	for(byte=2;byte<9;byte++){
		for(ref=0x01;ref<0x100;ref<<=1){
			CLK(0);
			PS2_DELAY;
			CLK(1);
		  if(DI) PS2_Data[byte] = ref|PS2_Data[byte];
			PS2_DELAY;
		}
    PS2_DELAY;
	}
	CS(1);
	PS2_DELAY;	
}
u8 IsPushed(u8 key){
	if( ((*(u16*)(PS2_Data+3)) &( 0x0001<<(key-1))) == 0 )return 1;
	return 0;
}
void ClearData(){
	for(int i=0; i<DATA_BYTES; i++){
		PS2_Data[i] = 0;
	}
}
int8_t PS2_GetLX(){
	return PS2_Data[7]-128;
}
int8_t PS2_GetRX(){
	return PS2_Data[5]-128;
}
int8_t PS2_GetLY(){
	return 127-PS2_Data[8];
}
int8_t PS2_GetRY(){
	return 127-PS2_Data[6];
}
void PS2_Test(){
	ClearData();
	PS2_ReadData();
	printf("ID0=%d ", PS2_Data[1]);
	printf("ID1=%d ", PS2_Data[2]);
	printf("(%d, ", PS2_GetLX());
	printf("%d); ", PS2_GetLY());
	printf("(%d ", PS2_GetRX());
	printf("%d) ", PS2_GetRY());
	printf("\r\n");
	for(int i=0; i<KEY_NUM; i++){
		if(IsPushed(i+1)) 
			printf("%s\r\n", (char*)KeyName[i]);
	}
}

u8 connect_flag = 0;
void PS2_Update(){
	ClearData();
	PS2_ReadData();
	if(PS2_Data[1]==115){
		connect_flag = 1;
	}
	else connect_flag = 0;
}

u8 IsPS2_OK(){
	return connect_flag;
}








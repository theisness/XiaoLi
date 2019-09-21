#include "lib_common.h"
#include "tim.h"

#define DELAY_TIMER htim3
extern TIM_HandleTypeDef DELAY_TIMER;

void delay_us(volatile u32 nus){
	HAL_TIM_Base_Start(&DELAY_TIMER);
  __HAL_TIM_SET_COUNTER(&DELAY_TIMER,0);
  while(__HAL_TIM_GET_COUNTER(&DELAY_TIMER)<nus){ };
  HAL_TIM_Base_Stop(&DELAY_TIMER);
}





void delay_init(){
	
}

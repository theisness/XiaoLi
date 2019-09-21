/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#define USART_REC_LEN 30
#define RXBUFFERSIZE 1
uint8_t USART_RX_BUF[USART_REC_LEN];     
uint16_t USART_RX_STA=0;       
uint8_t aRxBuffer[RXBUFFERSIZE];
#define UART_HANDER huart1
/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */
	HAL_UART_Receive_IT(&UART_HANDER, (uint8_t *)aRxBuffer, RXBUFFERSIZE);
	__HAL_UART_DISABLE_IT(&huart1,UART_IT_TC);

		__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);		//??????
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//??USART1????
		HAL_NVIC_SetPriority(USART1_IRQn,3,0);			//?????3,????3
  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
#include "lib_common.h"
void (*uart_callback)(uint8*) = NULL;
void SetUARTCallBack(void (*callback)(uint8*)){
	uart_callback = callback;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance==USART1){
		if((USART_RX_STA&0x8000)==0){
			if(USART_RX_STA&0x4000){
				if(aRxBuffer[0]!=0x0a)USART_RX_STA=0;
				else {
					USART_RX_BUF[USART_RX_STA&0X3FFF]=0;
					if( NULL!=uart_callback ) uart_callback(USART_RX_BUF);
					USART_RX_STA=0x0000;	 
				}
			}
			else {	
				if(aRxBuffer[0]==0x0d) USART_RX_STA|=0x4000;
				else{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=aRxBuffer[0] ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;
				}		 
			}
		}
	}
}
void USART1_IRQHandler(void){
		HAL_UART_IRQHandler(&UART_HANDER);	
    while (HAL_UART_GetState(&UART_HANDER) != HAL_UART_STATE_READY);
	while(HAL_UART_Receive_IT(&UART_HANDER, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK);
} 
#define SHOW_CHANNAL USART1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((SHOW_CHANNAL->SR&0X40)==0);//循环发送,直到发送完毕   
	SHOW_CHANNAL->DR = (u8) ch;      
	return ch;
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

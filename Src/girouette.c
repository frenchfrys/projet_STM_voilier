#include "stm32f1xx_hal.h"
#include "girouette.h"
#include "gpio.h"



void encoder_interface_mode(){
	
	__HAL_RCC_TIM3_CLK_ENABLE(); //démarrer horloge du TIM3
	TIM3->CCER &= ~(0x0011); //CC1P et CC2P disable
	
	TIM3->CCMR1 &= ~(0x0003) ;// CC1S = 00 TI1FP1 mapped on TI1
	TIM3->CCMR1 |= 0x0001; //CC1S = 01
	
	TIM3->CCMR1 &= ~(0x0300) ;// CC2S = 00 TI1FP2 mapped on TI2
	TIM3->CCMR1 |= 0x0100; //CC1S = 01 
	
	TIM3->CCER |= (0x0011); //CC1P et CC2P enable
	
	TIM3->SMCR &= ~(0x0007); //SMS = 000
	TIM3->SMCR |= (0x0001);  //SMS = 001 compteur/down TI2FP2 et TI1FP1
	
	TIM3->ARR = 0x05A0;
	TIM3->CR1 |= (0x0001); // CEN = 1 active le compteur
}

int read_angle(){
	return TIM3->CNT;
}

void connect_pin() {	

	  __HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_PA6_TIM3CH1;
		  /*Configure GPIO pin : PtPin */
  GPIO_PA6_TIM3CH1.Pin = GPIO_PIN_6;
  GPIO_PA6_TIM3CH1.Mode = GPIO_MODE_INPUT;
  //GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_PA6_TIM3CH1);

	GPIO_InitTypeDef GPIO_PA7_TIM3CH2;	
		  /*Configure GPIO pin : PtPin */
  GPIO_PA7_TIM3CH2.Pin = GPIO_PIN_7;
  GPIO_PA7_TIM3CH2.Mode = GPIO_MODE_INPUT;
  //GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_PA7_TIM3CH2);
	
		GPIO_InitTypeDef GPIO_PA5_ADCIN5;
		  /*Configure GPIO pin : PtPin */
  GPIO_PA5_ADCIN5.Pin = GPIO_PIN_5;
  GPIO_PA5_ADCIN5.Mode = GPIO_MODE_AF_INPUT;
  //GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_PA5_ADCIN5);
	}
#include "stm32f1xx_hal.h"
#include "girouette.h"

void encoder_interface_mode(){
	
	TIM3->CCER &= ~(0x0011); //CC1P et CC2P disable
	
	TIM3->CCMR1 &= ~(0x0003) ;// CC1S = 00 TI1FP1 mapped on TI1
	TIM3->CCMR1 |= 0x0001; //CC1S = 01
	
	TIM3->CCMR1 &= ~(0x0300) ;// CC2S = 00 TI1FP2 mapped on TI2
	TIM3->CCMR1 |= 0x0100; //CC1S = 01 
	
	TIM3->CCER |= (0x0011); //CC1P et CC2P enable
	
	TIM3->SMCR &= ~(0x0007); //SMS = 000
	TIM3->SMCR |= (0x0001);  //SMS = 001 compteur/down TI2FP2 et TI1FP1
	
	TIM3->CR1 |= (0x0001); // CEN = 1 active le compteur
}

int read_angle(){
	return TIM3->CNT;
}

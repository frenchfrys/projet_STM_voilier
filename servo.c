#include "stm32f10x.h"


int main (void)
{	
	//Activation horloge pour GPIOB et configuration broche PB.8 en sortie altern push-pull
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;		//permet d'activer le signal d'horloge pour le GPIOB (lié à la branche APB2)
	GPIOB->CRH &= ~(0xF << (4*0));						//PB.8 output altern push-pull
	GPIOB->CRH |= (0xB << (4*0));						//il faut (CRH.3 CRH.2 CRH.1 CRH.0) = 1011 pour une sortie altern push-pull
	
	
	
	//Activation horloge TIM4, détermination des paramètres ARR et PSC et lancement de TIM4
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;		//configuration de l'horloge
	
	TIM4->ARR = 14399;										//valeur d'autoreload ARR
	TIM4->PSC = 99;												//valeur du prescaler PSC
																				//Ttimer = Thorloge x (PSC + 1) x (ARR + 1) = 20ms (avec ARR et PSC choisis et Thorloge de 72 MHz) (ARR max = 65535 car registre 16 bits)
								
	TIM4->DIER |= 1<<0;										//autoritzation demande d'interruption cote peripherique
	NVIC->IP[30] = 2<<4;									//priorite d'interruption 2
	NVIC->ISER[0] |= 1<<30;								//
	
	TIM4->CR1 |= TIM_CR1_CEN;							//counter enable (CEN) dans TIM4 Control Register
																				//on lance le compteur
	
	
  // boucle de traitement
		while(1)
    {  
	
    }
		
}


void TIM4_IRQHandler(void) {
	TIM4->SR &= ~TIM_SR_UIF;						//on remet le bit à zéro
	GPIOA->ODR = GPIOA->ODR ^(1 << 5);							//XOR bit à bit entre 1 et le bit 5 du registre ODR du port GPIOA
																				//donc quelque soit le bit 5 de ODR (sortie du port 5) il est inversé car,
																						//XOR : F V -> V
																						//	  	V V -> F
																						//	  	V F -> V
																						//	  	F F -> F										
}


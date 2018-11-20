#ifndef MY_SERV
#define MY_SERV
#include "stm32f1xx_hal.h"

/*retourne l'angle désiré du servo-moteur 
en fonction de l'angle de la girouette*/
int compute_SM_angle(int a_girouette);

void config_PWM(GPIO_Init_TypeDef * GPIO_PA8_TIM1CH1, TIM_HandleTypeDef * TIM1,	TIM_OC_InitTypeDef *htim1);
	
/*commande l'angle du servo-moteur 
pour qu'il soit égal à l'angle passé en argument*/
//void control_SM_angle(int a_SM,....);


#endif
#include "stm32f1xx_hal.h"
#include "servomoteur.h"
#include "gpio.h"

int compute_SM_angle(int a_girouette)
{
	int theta; //angle du point d'écoute
	int x = (90/105);
	
	if( (a_girouette > (180+45)) & ( (a_girouette < (180-45)))) //près serré
	{
		theta = 0;
	}
	else if ((a_girouette > (360-30)) & ( (a_girouette < (30)))) //vent arrière
	{
		theta = 90;
	}
	else if ((a_girouette > (30)) & ( (a_girouette < (180-45))))
	{
		theta = (-x)*a_girouette;
	}
	else if ((a_girouette > (180+45)) & ( (a_girouette < (360-30))))
	{
		theta = x*a_girouette;
	}
	return theta;
}

void config_PWM(GPIO_Init_TypeDef *GPIO_PA8_TIM1CH1, TIM_HandleTypeDef *TIM1,	TIM_OC_InitTypeDef *htim1)
{
	  HAL_GPIO_Init(GPIOA, &GPIO_PA8_TIM1CH1);
		__HAL_RCC_TIM1_CLK_ENABLE(); //démarrer horloge du TIM1
		TIM1->CR1 |= (0x0001); // CEN = 1 active le compteur
}
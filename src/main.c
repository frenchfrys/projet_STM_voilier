#include "main.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "girouette.h"
#include "stm32f10x.h"


int main (void)
{	
	//ACCELEROMETRE
	//Configuration de l'ADC
	configure_GPIO_PC1_analog_input();
	configure_ADC_in11();
	set_watchdog_ADC();
	configuration_interruption_ADC();
	conversion_on();
	
	//GIROUETTE
	encoder_interface_mode();
	int a;
	
	// boucle de traitement
	while(1) {
		//GIROUETTE
		a=read_angle();
	}

}
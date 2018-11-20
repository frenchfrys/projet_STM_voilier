#include "main.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "girouette.h"
#include "stm32f10x.h"

<<<<<<< HEAD
int main(void)
<<<<<<< HEAD
{	
	connect_pin();
	encoder_interface_mode();

=======
{
=======

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
>>>>>>> 8e5fc8936e0d332288e578c30558c9c65cb92c9f
	encoder_interface_mode();
>>>>>>> 185686d99e5e1cd0006b060c33f7cb83fa893350
	int a;
	
	// boucle de traitement
	while(1) {
<<<<<<< HEAD
	a=read_angle();
<<<<<<< HEAD
	}
=======
}
	
>>>>>>> 185686d99e5e1cd0006b060c33f7cb83fa893350
=======
		//GIROUETTE
		a=read_angle();
	}

>>>>>>> 8e5fc8936e0d332288e578c30558c9c65cb92c9f
}
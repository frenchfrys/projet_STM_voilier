#include "main.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "girouette.h"

int main(void)
<<<<<<< HEAD
{	
	connect_pin();
	encoder_interface_mode();

=======
{
	encoder_interface_mode();
>>>>>>> 185686d99e5e1cd0006b060c33f7cb83fa893350
	int a;
	
	while(1) {
	a=read_angle();
<<<<<<< HEAD
	}
=======
}
	
>>>>>>> 185686d99e5e1cd0006b060c33f7cb83fa893350
}
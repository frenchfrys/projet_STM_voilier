#include "main.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "girouette.h"

int main(void)
{	
	connect_pin();
	encoder_interface_mode();

	int a;
	
	while(1) {
	a=read_angle();
	}
}
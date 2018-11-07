#include "stm32f10x.h"

void configure_GPIO_PC1_analog_input() { //CONFIGURATION GPIOC
  //On active les horloges de GPIOC
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	
  //Configure PC.1 en analog input : bits (0000) = 0x0 
  GPIOC->CRL &= ~(0xF << (1*4));	// mettre à 0 uniquement les bits 4 à 7  (mask = ~2_11110000)
}

void configure_ADC_in11() { //CONFIGURATION ADC1
	//Activation horloge ADC1
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	
	//Configurer horloge ADC a 12 MHz (divise par 6 la freq d'entree 72 MHz)
  RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;
	
	//Allumage ADC1 (il faut le faire pour lancer chaque conversion)
	ADC1->CR2 |= ADC_CR2_ADON;			//on met a 1 le bit ADON du registre CR2
	
	//Choix de la voie a lire
	ADC1->SQR1 &= ADC_SQR1_L;				//on veut lire seulement 1 voie
	ADC1->SQR3 |= 11;								//on indique la voie du ADC a lire (correspond a PC1)
	
	//Calibration du ADC pour eviter des erreurs
	ADC1->CR2 |= ADC_CR2_CAL;							//on met a 1 le bit CAL du registre CR2
	while ((ADC1->CR2 & ADC_CR2_CAL));		//on attend qu'il repasse a 0, donc bonne calibration
}


void set_watchdog_ADC() {
	ADC1->CR1->						/////p.238 awden
	ADC1->HTR = 2.37;							//high threshold register
	ADC1->LTR = 1.89;							//low threshold register
	
}

int convert_single() {
	
	ADC1->CR2 |=ADC_CR2_ADON;								//lancement de la conversion
	if (ADC1->SR & ADC_SR_AWD) {						//si analog watchdog event occurred
	
	}
	
	/*while(!(ADC1->SR & ADC_SR_EOC) ) {}			//attente fin de conversion
	ADC1->SR &= ~ADC_SR_EOC;								//validation de la conversion
	return ADC1->DR & ~((0x0F) << 12); 			//retour de la conversion		*/
}




int main (void)
{	
	//Configuration de l'ADC
	configure_GPIO_PC1_analog_input();
	configure_ADC_in11();
	set_watchdog_ADC();
	convert_single();
	
	// boucle de traitement
  while(1)
    {
			
    }

}

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
	ADC1->CR1 |= ADC_CR1_AWDIE;		//watchdog enabled
	ADC1->CR1 |= ADC_CR1_AWDEN;		//watchdog enabled on regular channels
	ADC1->CR1 &= ~ADC_CR1_JAWDEN;	//watchdog disabled on injected channels
	ADC1->CR1 |= ADC_CR1_AWDSGL;	//watchdog enabled on a single channel
	ADC1->CR1 &= ~ADC_CR1_AWDCH;	//selection de channel 11 pour le watchdog
	ADC1->CR1 |= 0xB;
	
	ADC1->HTR = 0xB7D;							//high threshold register
	ADC1->LTR = 0x929;							//low threshold register
	
}

void conversion_on() {
	
	ADC1->CR2 |=ADC_CR2_CONT;								//en mode continu
	ADC1->CR2 |=ADC_CR2_ADON;								//lancement de la conversion
	
	/*while(!(ADC1->SR & ADC_SR_EOC) ) {}			//attente fin de conversion
	ADC1->SR &= ~ADC_SR_EOC;								//validation de la conversion
	return ADC1->DR & ~((0x0F) << 12); 			//retour de la conversion		*/
}


void configuration_interruption_ADC()  {
	NVIC->IP[18] = 2<<4;			//le numéro de l'interruption de l'ADC est 18
														//Priorité 2, il faut la mettre dans les bits de poids fort de l'octet IP
	
	NVIC->ISER[0] |= 1<<18;		//Autorisation de recevoir la demande d'interruption côté NVIC
														//On met à 1 le bit correspondant au numero d'interruption (18) du vecteur ISER (set-enable register) de NVIC
	
}


void ADC1_2_IRQHandler(void) {
	NVIC->ICER[0] |= 1<<18;								//on disable l'autorisation de recevoir la demande d'interruption côté NVIC
	
																				//traitement a faire
	if (ADC1->DR > 0xAFF) {								//trop incline a gauche
		while (ADC1->DR > 0xB20) {}					//on attend qu'il revient a position normale
	}
	else {																//trop incline a droite
		while (ADC1->DR < 0x950) {}					//on attend qu'il revient a position normale
	}
	NVIC->ISER[0] |= 1<<18;								//on enable a nouveau l'autorisation de recevoir la demande d'interruption côté NVIC
	ADC1->SR &= ~ADC_SR_AWD;							//on remet le bit d'interruption a 0	
}

int main (void)
{	
	//Configuration de l'ADC
	configure_GPIO_PC1_analog_input();
	configure_ADC_in11();
	set_watchdog_ADC();
	configuration_interruption_ADC();
	conversion_on();
	
	// boucle de traitement
  while(1) {
//    if (ADC1->SR & ADC_SR_AWD) {						//si analog watchdog event occurred
//			ADC1->SR &= ~ADC_SR_AWD;							//on le remet a 0
//		}
			
    }

}

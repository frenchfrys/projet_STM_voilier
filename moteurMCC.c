#include "stm32f10x.h"


/*             BUT DU PROGRAMME

    - RECUPERER LES DONNEES EMISES PAR LA TELECOMMANDE RF
    - LES UTILISER POUR COMMANDER LE MOTEUR MCC (DIRECTION DU BATEAU)
            � CONTROLER LA VITESSE DE ROTATION (PWM)
            � CONTROLER LE SENS DE ROTATION (GAUCHE,DROITE : BINAIRE)

    Eventuellement : surveillance de la batterie.

*/

/*             EXPLICATIONS FONCTIONNEMENT

    la t�l�commande envoie un signal carr� de p�riode 20ms ou 25 ms (selon t�l�commande), soit f1 = 50 Hz ou f2 = 40 Hz

    le signal �mis d�fini la vitesse de rotation ET le sens de rotation du moteur, selon la valeur du rapport cyclique (T_on / T) avec T = 20 ou 25 ms

    valeurs de T_on (p�riode �tat haut) comprises entre 1 ms (minimum) et 2 ms (maximum)

    si T_on = 1.5 ms, alors on est en position neutre (on ne fait pas de rotation)
    si T_on compris entre 1.5 et 2 ms, alors on tourne dans un sens (vers la droite ? ). plus T_on est grand, plus on tourne vite.
    si T_on est compris entre 1 et 1.5 ms, alors on tourne dans l'autre sens. Plus T_on est petit, plus on tourne vite.

    Le signal est r�cup�r� par un r�cepteur RF fonctionnant largement au dessus de la fr�quence du signal : F_rf = 40 / 41 MHz

    Il faut r�cup�rer le signal (lire les donn�es) et trouver le rapport cyclique pour commander le moteur MCC.

*/


/*              PORTS ENTREE SORTIE

    port |  type  |             fonction principale                     |   Fonctions possibles (alternatives)

    PB6  : ENTREE : Channel 1 t�l�commande RF                           : I2C1_SCL / TIM4_CH1
    PB7  : ENTREE : Channel 2 t�l�commande RF                           : I2C1_SDA / TIM4_CH2

    PA2  : SORTIE : Sens de rotation moteur (binaire : 0 ou 1)          : USART2_TX / ADC_IN2 / TIM2_CH3
    PA1  : SORTIE : vitesse de rotation moteur (PWM)                    : USART2_RTS / ADC_IN1 / TIM2_CH2

*/

int resultat = 0;

int autoreload;				// valeur de l'autoreload (d�passement du timer)
int Ftimer;						// fr�quence timer

/*-------------------fonctions interruption-------------------------------------*/
					
void 	TIM2_IRQHandler (void)
{
	// mise � 0 du flag UIF (Flag d'interruption)
	TIM2->SR &= ~ TIM_SR_UIF;

	//lecture de l'�tat du channel 1 (r�cepteur RF)
	resultat = GPIOB->IDR & GPIO_IDR_IDR6 ;	
}
					
					
/*------------------------------------------------------------------------------*/	
					
					
					
					
					


int main (void)
{
	// initialisation horloges
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;		// Timer 2
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;		// GPIOA			
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;		// GPIOB	
	
	// TIMER qui permet de lire les donn�es du r�cepteur RF � chaque d�passement
	
	// configuration interruption timer 2
	NVIC->ISER[0] |= NVIC_ISER_SETENA_28;			// autorisation de d�clenchement de l'interruption n� 28
	NVIC->IP[28] = (2 << 4);						// priorit� � 2
	TIM2->DIER |= TIM_DIER_UIE; 					// autorisation d'effectuer une demande d'interruption n� 28
	
	// configuration timer 20	
	autoreload = 9;
	Ftimer = 100000;																	// fr�quence souhait�e en Hz (arbitraire : sert � r�cup�rer le signal de r�cepteur RF) : IL FAUT UNE RESOLUTION SUFFISANTE
	
	TIM2->ARR = autoreload;															// affectation de l'auto-reload
	TIM2->PSC = SystemCoreClock/(Ftimer * (autoreload+1)) - 1;						// affectation du prescaler (fr�quence d'entr�e / fr�quence de sortie)
	TIM2->CR1 |= TIM_CR1_CEN;														// activation de la sortie du prescaler (horloge d'entr�e du timer) -> d�marrage timer 2
	
  // boucle de traitement
  while(1)
    {


    }

}

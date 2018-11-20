#include "main.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"

#include "direction_bateau.h"



int autoreload;				// autoreload 
int Ftimer;						// fréquence souhaitée en sortie du timer 

float etat_haut_entree = 0;			// durée de l'état haut (en nombre de points par rapport à une fréquence d'échantillonnage)
float periode_entree = 0;				// durée de l'état haut + état bas = PERIODE du signal d'entrée
float rapport_cyclique_entree = 0;	// rapport cyclique d'entrée = état_haut/période_entrée (valeur normalement entre R_CYCLIQUE_LOW et R_CYCLIQUE_MAX
float rapport_cyclique_sortie = 0;	// rapport cyclique de sortie : envoyé au moteur (tension entre 0*12 et 1*12 V : valeur entre 0 et 1)

// structures caractérisant les entrées / sorties
GPIO_InitTypeDef GPIO_B6_B7;												// entrées channel 1 et 2 PWM	 -----> depuis "RF TELECO"

GPIO_InitTypeDef GPIO_A1;												// sortie signal PWM / sens rotation -----> vers moteur MCC
GPIO_InitTypeDef GPIO_A2;												// sortie signal PWM / sens rotation -----> vers moteur MCC

// création des structures pour TIM 4 et TIM 2
TIM_HandleTypeDef htim4;
TIM_ClockConfigTypeDef htim4_clockconfig;
TIM_SlaveConfigTypeDef htim4_slaveconfig;
TIM_MasterConfigTypeDef htim4_masterconfig;
TIM_IC_InitTypeDef htim4_ICinit;

TIM_HandleTypeDef htim2;												
TIM_ClockConfigTypeDef htim2_clockconfig;
TIM_MasterConfigTypeDef htim2_masterconfig;
TIM_OC_InitTypeDef htim2_OCinit;


void direction_enable_clock(void)
{
	__HAL_RCC_TIM4_CLK_ENABLE();	
	__HAL_RCC_TIM2_CLK_ENABLE();	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();	
}

void direction_config_ports(void)
{
		/*-----------------------------------------------------------------------------
							CONFIGURATION PORTS ENTREE SORTIE
	-----------------------------------------------------------------------------*/
	
	// entrée signal PWM ---> depuis "RF TELECO" (PB.6 / PB.7)
	GPIO_B6_B7.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_B6_B7.Mode = GPIO_MODE_INPUT;
	GPIO_B6_B7.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_B6_B7.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_B6_B7);
	
	// sortie signal PWM ---> vers moteur (PA.1 / PA.2)
	GPIO_A1.Pin = GPIO_PIN_1;
    GPIO_A1.Mode = GPIO_MODE_AF_PP;
    GPIO_A1.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_A1.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_A1);
		
	GPIO_A2.Pin = GPIO_PIN_2;
    GPIO_A2.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_A2.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_A2.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_A2);
}

void direction_config_timers (void)
{
	/*-----------------------------------------------------------------------------
							CONFIGURATION TIMERS 2 ET 4  
	-----------------------------------------------------------------------------*/
	
	/****************************************************************************
	// configuration TIMER 4 (entrée input-compare pour récupérer le signal PWM)
	*****************************************************************************/
	autoreload = 8999;
	Ftimer = 50;						// Fréquence d'échantillonage du capture
		
	htim4.Instance = TIM4;
	htim4.Init.Period = autoreload;
	htim4.Init.Prescaler = SystemCoreClock/(Ftimer * (autoreload+1)) - 1;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&htim4);

	htim4_clockconfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;				// utilisation horloge interne
	HAL_TIM_ConfigClockSource(&htim4, &htim4_clockconfig);
	
	HAL_TIM_IC_Init(&htim4); 		// mode input-capture 
	 
	// utilisation du mode "input PWM" : voir datasheet RM0008 p 383/384
	htim4_slaveconfig.SlaveMode = TIM_SLAVEMODE_RESET;		
	htim4_slaveconfig.InputTrigger = TIM_TS_TI1FP1;
	htim4_slaveconfig.TriggerPolarity = TIM_ICPOLARITY_RISING;
	htim4_slaveconfig.TriggerFilter = 0;
	HAL_TIM_SlaveConfigSynchronization(&htim4, &htim4_slaveconfig);
	
	htim4_masterconfig.MasterOutputTrigger = TIM_TRGO_RESET;
	htim4_masterconfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim4, &htim4_masterconfig);
	
	// partie réellement utile : configuration de "l'input-capture" : gestion de la récupération du signal d'entrée PWM
	htim4_ICinit.ICPolarity = TIM_ICPOLARITY_RISING;									// déclenchement de la capture sur le front montant du signal
	htim4_ICinit.ICSelection = TIM_ICSELECTION_DIRECTTI;								// configuration des channels "directe" : voir doc
	htim4_ICinit.ICPrescaler = TIM_ICPSC_DIV1;													// capture à chaque front montant (on peut aussi capturer tous les 2/4 fronts montants)
	htim4_ICinit.ICFilter = 0;
	HAL_TIM_IC_ConfigChannel(&htim4, &htim4_ICinit, TIM_CHANNEL_1);			// channel 1 : capture front montant --> rapport cyclique --> port PB.6 info 1
	
	htim4_ICinit.ICPolarity = TIM_ICPOLARITY_FALLING;									// channel 2 : capture front descendant --> période --> port PB.6 info 2
	htim4_ICinit.ICSelection = TIM_ICSELECTION_INDIRECTTI;								// configuration des channels "directe" : voir doc
	HAL_TIM_IC_ConfigChannel(&htim4, &htim4_ICinit, TIM_CHANNEL_2);
	
	// page 366 RM 0008 : on a un seul pin connecté en entrée : PB.6 . Les deux infos arrivent sur le même channel 1 physiquement : il faut router l'info du channel 1 sur IC1 et IC2. 
	// il faut donc une choisir TIFP1 pour le multiplexeur de IC1 et TI1FP2 pour le multiplexeur de IC2 --> même channel entrée : 2 channels sortie ! etat_haut et période état_bas
	
	
	/****************************************************************************
	// configuration TIMER 2 (sortie signal PWM vers moteur)
	*****************************************************************************/
	autoreload = 999;
	Ftimer = 72000;								// fréquence que l'on veut en sortie.
	
	htim2.Instance = TIM2;
	htim2.Init.Period = autoreload;
	htim2.Init.Prescaler = SystemCoreClock/(Ftimer * (autoreload+1)) - 1;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&htim2);
	
	htim2_clockconfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;						// utilisation horloge interne
	HAL_TIM_ConfigClockSource(&htim2, &htim2_clockconfig);
	
	HAL_TIM_PWM_Init(&htim2);														// initialisation mode : génération de PWM
	
	htim2_masterconfig.MasterOutputTrigger = TIM_TRGO_RESET;
	htim2_masterconfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim2, &htim2_masterconfig);
	
	// partie réellement utile : configuration du mode "génération de signal PWM" de base
	rapport_cyclique_sortie = 0.0;													// rapport cyclique initial voulu en % (ici : 0 comme ça le moteur ne tourne pas) 
	
	htim2_OCinit.OCMode = TIM_OCMODE_PWM1;										// utilisation du mode PWM
	htim2_OCinit.Pulse = ((rapport_cyclique_sortie) *(autoreload + 1)) - 1;		// définition du rapport cyclique : "A DEFINIR AVEC LA RECUPERATION DU SIGNAL"
	htim2_OCinit.OCPolarity = TIM_OCPOLARITY_HIGH;
	htim2_OCinit.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&htim2, &htim2_OCinit, TIM_CHANNEL_2);		// utilisation du TIM channel 2 : correspond au port PA.1
	
}

void direction_start_timers_IT (void)
{
	/*----------------------------------------------------------------------
									DEMARAGE TIMER, IT, ETC...
	----------------------------------------------------------------------*/				
	HAL_NVIC_SetPriority (TIM4_IRQn,1,0);												// définition de la priorité de l'interruption "dépassement de TIM4" à 1
	HAL_NVIC_EnableIRQ(TIM4_IRQn);														// activation de l'interruption "TIM 4 global interrupt"

	// Activation démarage avec interruption : TIM4
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2);						// DEMARRAGE du timer 4 --> capture de la période totale et de l'état haut (Temps état haut) du signal en entrée
	
	HAL_TIM_PWM_Start(& htim2, TIM_CHANNEL_2);						// DEMARRAGE du Timer 2 (voir HAL page 516) --> génération de signal PWM
}

void direction_traitement_donnees (void)
{
		if(periode_entree != 0 && etat_haut_entree != 0)
		{
					rapport_cyclique_entree = etat_haut_entree / periode_entree;			// calcul du rapport cyclique obtenu par le récepteur RF --> valeur réelle (pas de pourcentage)
			
					// selon le rapport cyclique : traitement du sens de rotation du moteur + vitesse du moteur
					// cas 1 : Rapport cyclique entrée = 7.5 % -----> Moteur à l'arrêt --> rapport cyclique de sortie = 0
					// cas 2 : Rapport cyclique entrée > 7.5 % (et théoriquement < 10 %) ---> Moteur en rotation vers la droite plus ou moins vite --> rapport cyclique de sortie E [0;1]
					// cas 3 : Rapport cyclique entrée = 7.5 % (et théoriquement > 5 %) ---> Moteur en rotation vers la gauche plus ou moins vite --> rapport cyclique de sortie E [0;1]
					
					if (rapport_cyclique_entree > SEUIL_BAS && rapport_cyclique_entree < SEUIL_HAUT )		// cas 1
					{
						rapport_cyclique_sortie = 0;		// rapport cyclique de sortie PAS EN POURCENTAGE --> VALEUR REELLE : important pour les calculs
						htim2_OCinit.Pulse = ((rapport_cyclique_sortie) *(autoreload + 1)) - 1;						
						HAL_TIM_PWM_ConfigChannel(&htim2, &htim2_OCinit, TIM_CHANNEL_2);		
						HAL_TIM_PWM_Start(& htim2, TIM_CHANNEL_2);						// DEMARRAGE du Timer 2 (voir HAL page 516) --> génération de signal PWM
					}
					else if (rapport_cyclique_entree > SEUIL_HAUT)		// cas 2
					{			
						// ci dessous -> on connait rp_entree max (13) et rp_entree min (11.5), ainsi que rp_sortie max (1) et rp_sortie min (0)
						// on en déduit avec le coefficient directeur une équation de droite linéaire par rapport à laquelle le rp_sortie varie entre 0 et 1
						rapport_cyclique_sortie = 66.6666 * rapport_cyclique_entree - 7.66;			// rapport cyclique de sortie PAS EN POURCENTAGE --> VALEUR REELLE : important pour les calculs
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);			// choix du sens de rotation à droite
						htim2_OCinit.Pulse = ((rapport_cyclique_sortie) *(autoreload + 1)) - 1;		// mise à jour du rapport cyclique de sortie (A VERIFIER)
						HAL_TIM_PWM_ConfigChannel(&htim2, &htim2_OCinit, TIM_CHANNEL_2);	
						HAL_TIM_PWM_Start(& htim2, TIM_CHANNEL_2);						// DEMARRAGE du Timer 2 (voir HAL page 516) --> génération de signal PWM
					}
					else if (rapport_cyclique_entree < SEUIL_BAS)		// cas 3
					{
						// ci dessous -> on connait rp_entree max (10) et rp_entree min (11.5), ainsi que rp_sortie max (1) et rp_sortie min (0)
						// on en déduit avec le coefficient directeur une équation de droite linéaire par rapport à laquelle le rp_sortie varie entre 0 et 1
						rapport_cyclique_sortie = - 66.6666 * rapport_cyclique_entree + 7.66;  // rapport cyclique de sortie PAS EN POURCENTAGE --> VALEUR REELLE : important pour les calculs
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);		// choix du sens de rotation à gauche 
						htim2_OCinit.Pulse = ((rapport_cyclique_sortie) *(autoreload + 1)) - 1;		
						HAL_TIM_PWM_ConfigChannel(&htim2, &htim2_OCinit, TIM_CHANNEL_2);		// utilisation du TIM channel 2 : correspond au port PA.1
						HAL_TIM_PWM_Start(& htim2, TIM_CHANNEL_2);						// DEMARRAGE du Timer 2 (voir HAL page 516) --> génération de signal PWM
					}
		}
}



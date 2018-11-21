#ifndef DIRECTION_BATEAU__H
#define DIRECTION_BATEAU__H


#include "main.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"

#define R_CYCLIQUE_LOW 10
#define R_CYCLIQUE_MID 11.5
#define R_CYCLIQUE_MAX 13

#define SEUIL_BAS (R_CYCLIQUE_MID-0.1) /100
#define SEUIL_HAUT (R_CYCLIQUE_MID+0.1) /100

void direction_enable_clock(void);	// activation des horloges nécessaires pour le déplacement du bateau. 
void direction_config_ports(void);	// configuration ports entrées / sorties : récupération / génération PWM -> PB.6 , PB.7, PA.1, PA.2
void direction_config_timers (void);	// configuration des timers 2 et 4
void direction_start_timers_IT (void);	// démarrage des timers 2 et 4
void direction_traitement_donnees (void); // gestion du PWM d'entrée et de sortie


#endif

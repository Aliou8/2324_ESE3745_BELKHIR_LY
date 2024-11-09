/**
 * @file moteur.c
 * @author Aliou LY (aliou.ly@ensea.fr)
 * @brief Ce fichier contient les fonction pour controler le moteur via les PWM generés
 * @version 0.1
 * @date 2024-11-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "moteur.h"
#include "tim.h"


/**
 * @brief Une fonction qui permet de changer progressivement le rapport cyclique du PWM
 * 
 * @param alpha Nouvelle valeur du rapport cyclique souhaité (entre 0 et 100)
 */
static void setPWMsDutyCycle(int alpha)
{
    int alphaActuel = __HAL_TIM_GetCompare(&htim1, TIM_CHANNEL1); // Obtient le rapport cyclique actuel de TIM_CHANNEL1
    
    // Ajustement progressif du rapport cyclique
    if (alpha < alphaActuel)
    {
        // Si la nouvelle valeur est inférieure à l'actuelle, on la diminue progressivement
        while (alphaActuel > alpha)
        {
            alphaActuel--; // Décrémente alphaActuel pour atteindre la cible
            __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL1, alphaActuel);
            __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL2, 100 - alphaActuel); // Valeur complémentaire pour TIM_CHANNEL2
            HAL_Delay(1); // Ajoute un léger délai pour la transition progressive
        }
    }
    else
    {
        // Si la nouvelle valeur est supérieure à l'actuelle, on l'augmente progressivement
        while (alphaActuel < alpha)
        {
            alphaActuel++; // Incrémente alphaActuel pour atteindre la cible
            __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL1, alphaActuel);
            __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL2, 100 - alphaActuel); // Valeur complémentaire pour TIM_CHANNEL2
            HAL_Delay(1); // Ajoute un léger délai pour la transition progressive
        }
    }
}


/**
 * @brief Cette fonction permet d'ajuster la vitesse du moteur . Recevant un commande de type char , on conertis la commande en int
 * et aussi on limite la vitesse en 0 et 100%
 * 
 * @param cmd  : la vitesse souhaitée transmise par le UART
 */

void motorSpeed(char* cmd)
{

	double alpha = atoi(cmd);
	if (alpha > 100){ // on teste si notre valeur désirée est plus grande que la vitessse maximale
		alpha = 100;
	}
	if (alpha < 0){ // on teste si notre valeur désirée est plus grande en valeur absolue que la vitessse maximale
		alpha = 0;
	}
	alpha = alpha/100;
	
}


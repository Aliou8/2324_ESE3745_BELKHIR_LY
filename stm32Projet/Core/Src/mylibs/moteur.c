/**
 * @file moteur.c
 * @author Aliou LY (aliou.ly@ensea.fr)
 * @brief Implémentation des fonctions de contrôle du moteur via les signaux PWM.
 * @version 0.1
 * @date 2024-11-09
 * 
 * Ce fichier contient les fonctions permettant de démarrer, arrêter et ajuster la vitesse du moteur.
 * La vitesse est contrôlée par un signal PWM, dont le rapport cyclique est ajusté progressivement pour
 * éviter les transitions brutales.
 */

#include "mylibs/moteur.h"
#include "tim.h"
#include <stdlib.h>

/**
 * @brief Change progressivement le rapport cyclique PWM pour atteindre la valeur souhaitée.
 * 
 * Cette fonction permet de changer le rapport cyclique de manière douce pour éviter les changements
 * brusques dans la vitesse du moteur.
 * 
 * @param alpha Nouvelle valeur du rapport cyclique souhaité 
 */

static void setPWMsDutyCycle(int alpha)
{
    int alphaActuel = __HAL_TIM_GetCompare(&htim1, TIM_CHANNEL_1); // Obtient le rapport cyclique actuel de TIM_CHANNEL1

    // Ajustement progressif du rapport cyclique
    if (alpha < alphaActuel)
    {
        // Si la nouvelle valeur est inférieure à l'actuelle, on la diminue progressivement
        while (alphaActuel > alpha)
        {
            alphaActuel--; // Décrémente alphaActuel pour atteindre la cible
            __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, alphaActuel);
            __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, ALPHA_MAX - alphaActuel);
            HAL_Delay(1); 
        }
    }
    else
    {
        // Si la nouvelle valeur est supérieure à l'actuelle, on l'augmente progressivement
        while (alphaActuel < alpha)
        {
            alphaActuel++; // Incrémente alphaActuel pour atteindre la cible
            __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, alphaActuel);
            __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, ALPHA_MAX - alphaActuel);
            HAL_Delay(1);
        }
    }
}

/**
 * @brief Définit la vitesse du moteur en fonction de la commande reçue.
 * 
 * Cette fonction reçoit une commande de type char* représentant la vitesse souhaitée, la convertit en un entier,
 * et la limite entre 0 et VITESSE_MAX. Elle ajuste ensuite le rapport cyclique PWM.
 * 
 * @param cmd La commande de vitesse souhaitée transmise par UART 
 */

void moteurSetSpeed(char* cmd)
{
    double vitesse = atoi(cmd);
    if (vitesse > VITESSE_MAX)  // Limite la vitesse à la valeur maximale
    {
        vitesse = VITESSE_MAX;
    }
    if (vitesse < 0)  // Limite la vitesse à la valeur minimale
    {
        vitesse = 0;
    }
    uint8_t alpha = (vitesse * ALPHA_MAX) / VITESSE_MAX;  // Convertit la vitesse en rapport cyclique PWM
    setPWMsDutyCycle(alpha);
}

/**
 * @brief Démarre le moteur en fixant un rapport cyclique initial de 50%.
 * 
 * Cette fonction initialise le moteur avec un rapport cyclique correspondant à une vitesse de 50%.
 * Elle configure les canaux TIM_CHANNEL1 et TIM_CHANNEL2 avec des valeurs complémentaires pour démarrer le moteur.
 */
void moteurStart(void)

{
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, ALPHA_0);
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, ALPHA_MAX - ALPHA_0);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
}

/**
 * @brief Arrête le moteur en fixant le rapport cyclique à 0%.
 * 
 * Cette fonction stoppe le moteur en mettant le rapport cyclique des canaux TIM_CHANNEL1 et TIM_CHANNEL2 à 0.
 * Elle arrête le signal PWM pour chaque canal.
 */
void moteurStop(void)

{
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, ALPHA_MIN);
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, ALPHA_MIN);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
}

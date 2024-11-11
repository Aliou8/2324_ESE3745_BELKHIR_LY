/**
 * @file moteur.h
 * @author Aliou LY (aliou.ly@ensea.fr)
 * @brief Interface de contrôle du moteur, permettant de démarrer, arrêter, et ajuster la vitesse du moteur
 *        via des signaux PWM.
 * @version 0.1
 * @date 2024-11-11
 * 
 * Définit des constantes et des fonctions permettant de contrôler le moteur. Le moteur est contrôlé par un signal PWM
 * dont le rapport cyclique ajuste la vitesse. La commande de vitesse est transmise en pourcentage et convertie
 * en valeur PWM.
 * 
 * @note Ce fichier doit être utilisé avec le fichier d'implémentation `moteur.c`.
 * 
 */

#ifndef __MOTEUR__
#define __MOTEUR__

#include "tim.h"

// Définition des constantes pour le rapport cyclique PWM
#define ALPHA_MAX (TIM1.ARR) ///< Valeur maximale du rapport cyclique (PWM à 100%)
#define ALPHA_MIN 0           ///< Valeur minimale du rapport cyclique (PWM à 0%)
#define ALPHA_0   (ALPHA_MAX / 2) ///< Valeur moyenne du rapport cyclique (PWM à 50%)
#define VITESSE_MAX 3000      ///< Vitesse maximale définie pour le moteur (en pourcentage ou unité)

/// @brief Démarre le moteur avec un rapport cyclique initial de 50%.
void moteurStart(void);

/// @brief Arrête le moteur en fixant le rapport cyclique à 0%.
void moteurStop(void);

/// @brief Ajuste la vitesse du moteur en fonction de la commande reçue.
/// @param cmd : La commande de vitesse .
void moteurSetSpeed(char * cmd);

#endif // __MOTEUR__

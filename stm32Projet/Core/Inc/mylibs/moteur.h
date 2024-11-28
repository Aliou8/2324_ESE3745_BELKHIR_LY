/**
 * @file moteur.h
 * @author Aliou LY
 * @brief Interface de contrôle du moteur, permettant de démarrer, arrêter, et ajuster la vitesse du moteur
 * via des signaux PWM.
 * @version 0.1
 * @date 2024-11-11
 * 
 * Ce fichier fournit les constantes, structures et prototypes nécessaires pour contrôler un moteur.
 * Le moteur est contrôlé par un signal PWM, et des fonctions supplémentaires permettent de mesurer
 * le courant, la position et la vitesse du moteur.
 * 
 * @note Ce fichier doit être utilisé avec le fichier d'implémentation `moteur.c`.
 */

#ifndef __MOTEUR__
#define __MOTEUR__

#include "tim.h"

/**
 * @brief Valeur maximale du rapport cyclique PWM (correspond à 100%).
 */
#define ALPHA_MAX (TIM1->ARR)

/**
 * @brief Valeur minimale du rapport cyclique PWM (correspond à 0%).
 */
#define ALPHA_MIN 0

/**
 * @brief Valeur moyenne du rapport cyclique PWM (correspond à 50%).
 */
#define ALPHA_0 (ALPHA_MAX / 2)

/**
 * @brief Vitesse maximale définie pour le moteur, exprimée RPM .
 */
#define VITESSE_MAX 3000

/**
 * @brief Taille du tampon ADC utilisé pour les conversions en mode DMA.
 */
#define ADC_BUFFER_SIZE 1

/**
 * @brief Structure contenant les paramètres du régulateur PID.
 */
typedef struct {
    float Kp;     ///< Gain proportionnel.
    float Ki;     ///< Gain intégral.
    float Kd;     ///< Gain dérivé.
    float dt;     ///< Intervalle de temps en secondes.
} PID_parameter_t;

/**
 * @brief Démarre le moteur avec un rapport cyclique initial de 50%.
 */
void moteurStart(void);

/**
 * @brief Arrête le moteur en fixant le rapport cyclique à 0%.
 */
void moteurStop(void);

/**
 * @brief Ajuste la vitesse du moteur en fonction de la commande reçue.
 *
 * Cette fonction prend une commande sous forme de chaîne de caractères,
 * la convertit en valeur numérique, et ajuste la vitesse du moteur via le PWM.
 *
 * @param cmd La commande de vitesse sous forme de chaîne de caractères.
 */
void moteurSetSpeed(char *cmd);

/**
 * @brief Affiche le courant mesuré par l'ADC sur la console.
 */
void displayCurrent(void);

/**
 * @brief Démarre l'encodeur pour mesurer la position et la vitesse du moteur.
 */
void encodeurStart(void);

/**
 * @brief Calcule et retourne le courant mesuré par l'ADC.
 *
 * @return La valeur du courant mesuré en ampères.
 */
float getCurrent(void);

/**
 * @brief Calcule et retourne la vitesse du moteur en tours par minute (RPM).
 *
 * Cette fonction utilise une position précédente pour calculer la vitesse actuelle.
 *
 * @param oldpos La dernière position connue du moteur.
 * @return La vitesse du moteur en RPM.
 */
float getSpeed(int16_t oldpos);

/**
 * @brief Initialise l'ADC en mode DMA avec un déclenchement périodique par TIMER.
 */
void ADC_DMA_Init(void);

/**
 * @brief Affiche la vitesse du moteur calculée sur la console.
 */
void displaySpeed(void);

/**
 * @brief Retourne la position actuelle du moteur à partir de l'encodeur.
 *
 * @return La position actuelle du moteur.
 */
int16_t getPos(void);

#endif // __MOTEUR__

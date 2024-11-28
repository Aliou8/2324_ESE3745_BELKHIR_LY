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
#include <stdio.h>
#include <stdlib.h>
#include "adc.h"

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
			HAL_Delay(20);
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
			HAL_Delay(20);
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

void moteurSetSpeed(char *cmd)
{
	float vitesse = atoi(cmd);
	if (vitesse > VITESSE_MAX) // Limite la vitesse à la valeur maximale
	{
		vitesse = VITESSE_MAX;
	}
	if (vitesse < 0) // Limite la vitesse à la valeur minimale
	{
		vitesse = 0;
	}
	uint32_t alpha = 0.5*(1+vitesse/VITESSE_MAX)*ALPHA_MAX; // Convertit la vitesse en rapport cyclique PWM
	setPWMsDutyCycle(alpha);
}

/**
 * @brief Démarre le moteur en fixant un rapport cyclique initial de 50%.
 *
 * Cette fonction initialise le moteur avec un rapport cyclique correspondant à une vitesse de 50%.
 * Elle configure les canaux TIM_CHANNEL1 et TIM_CHANNEL2 et les complémentaires pour démarrer le moteur.
 */
void moteurStart(void)
{
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, ALPHA_0);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, ALPHA_MAX - ALPHA_0);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
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

/// Résolution maximale de l'ADC (12 bits, soit 4095).
static float ADC_12B = 4095.0;

/// Tension de référence de l'ADC en volts.
static float Vref = 3.3;

/// Tension de décalage (offset) en volts.
static float Voff = 1.95;

/// Constante de conversion tension-courant.
static float Ks = 0.05;

/// Tampon DMA pour stocker les données ADC.
static uint16_t adc_buffer[ADC_BUFFER_SIZE];

/**
 * @brief Lit la valeur actuelle du courant via le tampon DMA et l'affiche sur la console.
 *
 * Cette fonction lit la dernière valeur convertie par l'ADC (gérée en arrière-plan par le DMA),
 * calcule le courant correspondant à l'aide des paramètres calibrés, puis
 * affiche la valeur du courant sur la console via `printf`.
 */
void displayCurrent(void)

{
	float I = getCurrent();

	// Afficher le courant mesuré
	printf("Current = %f A\r\n", I);
}

/**
 * @brief Initialise l'ADC en mode DMA avec déclenchement par TIMER.
 *
 * Configure le DMA pour transférer les données ADC dans un tampon
 * et démarre le TIMER pour assurer un déclenchement périodique des conversions ADC.
 */
void ADC_DMA_Init(void)

{
	// Démarre le DMA pour recevoir les données ADC
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, ADC_BUFFER_SIZE);

	// Démarre le timer pour déclencher les conversions ADC périodiquement
	HAL_TIM_Base_Start(&htim1);
}

/**
 * @brief Calcule le courant mesuré à partir des données ADC.
 *
 * Cette fonction lit la dernière valeur du tampon ADC (rempli automatiquement par DMA),
 * calcule la tension de sortie correspondante, puis convertit cette tension en courant
 * en utilisant une formule calibrée.
 *
 * @return Le courant mesuré en ampères (float).
 */
float getCurrent(void)
{
	// Lire la dernière valeur convertie par l'ADC
	float ADC_value = adc_buffer[0];

	// Calculer la tension de sortie à partir de la valeur ADC
	float Vout = ADC_value*Vref/ ADC_12B;

	// Calculer le courant à partir de la tension de sortie
	float I = (Vout- Voff)/Ks;

	// Retourner le courant calculé
	return I;
}


/**
 * @brief Fréquence d'échantillonnage de la vitesse en Hz.
 */
static float Fe = 100.0;

/**
 * @brief Retourne la position actuelle du moteur à partir du compteur du Timer.
 *
 * Cette fonction lit la valeur actuelle du compteur du Timer (TIM3), configuré en mode encodeur,
 * pour déterminer la position du moteur.
 *
 * @return La position actuelle du moteur en tant qu'entier 16 bits signé.
 */
int16_t getPos(void)
{
	int16_t position = TIM3->CNT;
	return position;
}


/**
 * @brief Calcule la vitesse de rotation du moteur en tours par minute (RPM).
 *
 * Cette fonction utilise la différence entre une position précédente et la position actuelle
 * pour calculer la vitesse de rotation du moteur. La vitesse est exprimée en tours par minute (RPM)
 * en utilisant la fréquence d'échantillonnage du système (Fe).
 *
 * @param oldpos La position précédente du moteur, obtenue à partir de `getPos()`.
 * @return La vitesse de rotation calculée en tours par minute (RPM) sous forme de nombre flottant.
 */
float getSpeed(int16_t oldpos)
{
	int16_t pos = getPos();
	return (float)(pos - oldpos) * Fe * 60;
}


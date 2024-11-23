# 2425_ESE3745_BELKHIR_LY
TP Actionneur et automatique appliquée


# Séance 1 - Commande MCC basique

## 1. Fréquence du Processeur : 170 MHz
Le processeur fonctionne à une fréquence de 170 MHz, soit \( 170 	imes 10^6 \) Hz.

## 2. Calcul de la Fréquence de Sortie PWM
Pour générer un signal PWM avec une fréquence de 20 kHz, nous devons configurer deux registres du timer STM32 : ARR (Auto-Reload Register) et PSC (Prescaler).

Le calcul pour déterminer ces paramètres se fait comme suit :
\[
f_{	ext{timer}} = rac{f_{	ext{horloge}}}{(PSC + 1) 	imes (ARR + 1)}
\]
Où :
- \( f_{	ext{timer}} \) est la fréquence du timer, que nous souhaitons égaler à 20 kHz.
- \( f_{	ext{horloge}} = 170 \, 	ext{MHz} \) est la fréquence d'horloge du processeur.
- \( PSC \) est le prescaler du timer.
- \( ARR \) est l'auto-reload register du timer.

## 3. Choix des Paramètres : ARR et PSC
Nous voulons une fréquence de sortie de 20 kHz. 
Pour avcoir une resolution de 10bits : ARR de 1024-1 , nous pouvons calculer le prescaler :

\[
20 \, 	ext{kHz} = rac{170 \, 	ext{MHz}}{(PSC + 1) 	imes (1023 + 1)}
\]

En résolvant pour \( PSC \), nous trouvons :
\[
PSC + 1 = rac{170 	imes 10^6}{20 	imes 10^3 	imes 1024} = 8.31
\]
\[
PSC = 7.31
\]

Ainsi, pour obtenir une fréquence de 20 kHz, nous choisissons :
- **ARR = 1023**
- **PSC = 7**

Cela donne une fréquence de 20.75 kHz, ce qui est proche de notre cible de 20 kHz.

---

## 4. Calcul des Dead Times
Le dead time est  calculé en ajoutant deux temps : le Turn-Off Delay Time (35 ns) et le Fall Time (39 ns), ce qui donne un total de 74 ns. on choisit un dead time de 100 ns.

### 4.1 Fréquence d'horloge du processeur : 170 MHz
Cela donne un temps par tick d'horloge de :
\[
	ext{Temps par tick} = rac{1}{170 	imes 10^6} = 5.88 \, 	ext{ns}
\]

### 4.2 Dead Time de 100 ns
Nous voulons un dead time de 100 ns. Le nombre de ticks correspondant est calculé comme suit :
\[
	ext{Nombre de ticks} = rac{100 \, 	ext{ns}}{5.88 \, 	ext{ns/tick}} pprox 17 \, 	ext{ticks}
\]

Ainsi, le **dead time** doit être réglé à **17 ticks**.

---

## 5. Configuration du Timer en Mode Complémentaire Décalé
Pour générer un signal PWM avec complémentarité décalée, nous devons configurer le timer en **mode center-aligned**. Cela signifie que le timer va utiliser un comptage à partir de zéro vers une valeur maximale, puis compter de manière descendante et aussi s'assurer que la condition **alpha1 + alpha2 = 1** est remplie.

---

## 6. Gestion des Problèmes d'Appel de Courant
Lorsque vous changez rapidement le rapport cyclique de la PWM, cela peut entraîner un appel de courant élevé.

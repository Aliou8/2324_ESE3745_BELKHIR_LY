# 2425_ESE3745_BELKHIR_LY
TP Actionneur et automatique appliquée

# Séance 1 - Commande MCC basique

## 1. Fréquence du Processeur : 170 MHz
Le processeur fonctionne à une fréquence de 170 MHz, soit \( 170 \times 10^6 \) Hz.

## 2. Calcul de la Fréquence de Sortie PWM
Pour générer un signal PWM avec une fréquence de 20 kHz, nous devons configurer deux registres du timer STM32 : ARR (Auto-Reload Register) et PSC (Prescaler).

Le calcul pour déterminer ces paramètres se fait comme suit :

\[
f_{\text{timer}} = \frac{f}{(PSC + 1) \times (ARR + 1)}
\]

## 3. Choix des Paramètres : ARR et PSC
Nous voulons une fréquence de sortie de 20 kHz. 
Pour avoir une résolution de 10 bits : ARR de 1024-1, nous pouvons calculer le prescaler :

\[
20\text{kHz} = \frac{170\text{MHz}}{(PSC + 1) \times (1023 + 1)}
\]

En résolvant pour \( PSC \), nous trouvons :

\[
PSC + 1 = \frac{170 \times 10^6}{20 \times 10^3 \times 1024} = 8.31
\]

Ainsi,

\[
PSC = 7.31
\]

Ainsi, pour obtenir une fréquence de 20 kHz, nous choisissons :
- **ARR = 1023**
- **PSC = 7**

Cela donne une fréquence de 20.75 kHz, ce qui est proche de notre cible de 20 kHz.

---

## 4. Calcul des Dead Times
Le dead time est calculé en ajoutant deux temps : le Turn-Off Delay Time (35 ns) et le Fall Time (39 ns), ce qui donne un total de 74 ns. On choisit un dead time de 100 ns.

### 4.1 Fréquence d'horloge du processeur : 170 MHz
Cela donne un temps par tick d'horloge de :

\[
\text{Nombre de ticks} = \frac{100 \, \text{ns}}{5.88 \, \text{ns/tick}} \approx 17 \, \text{ticks}
\]

### 4.2 Dead Time de 100 ns
Nous voulons un dead time de 100 ns. Le nombre de ticks correspondant est calculé comme suit :

\[
\text{Nombre de ticks} = \frac{100 \, \text{ns}}{5.88 \, \text{ns/tick}} \approx 17 \, \text{ticks}
\]

Ainsi, le **dead time** doit être réglé à **17 ticks**.

---

## 5. Configuration du Timer en Mode Complémentaire Décalé
Pour générer un signal PWM avec complémentarité décalée, nous devons configurer le timer en **mode center-aligned**. Cela signifie que le timer va utiliser un comptage à partir de zéro vers une valeur maximale, puis compter de manière descendante et aussi s'assurer que la condition **alpha1 + alpha2 = 1** est remplie.

---

Lorsqu'on change rapidement le rapport cyclique de la PWM, cela peut entraîner un appel de courant élevé.

# Séance 2 - Commande en boucle ouverte, mesure de Vitesse et de courant

## Commande de la vitesse

## Mesure du courant

La mesure du courant se fait avec `Bus_Imes` connecté sur le PA1, qui est relié à `ADC1_IN2`.

La formule qui relie la tension mesurée et le courant est  :

\[
V_{\text{out}} = V_{\text{off}} + K_s \times I
\]

- \( V_{\text{out}} \) : Tension mesurée (en volts)
- \( V_{\text{off}} \) : Tension offset (1.65 V)
- \( K_s \): Constante (50 mV/A, soit 0.05 V/A)
- \( I \) : Courant mesuré (en ampères)

### 2. Conversion de la valeur mesurée en courant

L'ADC mesure la tension \( V_{\text{out}} \), qui est ensuite convertie en courant. L'ADC a une résolution de 12 bits, ce qui signifie qu'il peut donner une valeur entre 0 et \( 2^{12} - 1 = 4095 \).

#### Étapes de conversion :

1. **Calculer la tension mesurée \( V_{\text{out}} \)** : L'ADC fournit une valeur numérique entre 0 et 4095. Pour obtenir la tension correspondante, nous devons utiliser la référence de l'ADC. Si la référence est de 3.3V, la formule pour obtenir la tension mesurée est :

   \[
   V_{\text{out}} = \frac{\text{ADC\_value}}{4095} \times V_{\text{ref}}
   \]

   - \( \text{ADC\_value} \) : Valeur lue par l'ADC (entre 0 et 4095)
   - \( V_{\text{ref}} \) : Tension de référence de l'ADC (3.3V typiquement)

2. **Calculer le courant à partir de la tension mesurée** : Une fois que vous avez \( V_{\text{out}} \), vous pouvez résoudre la relation :

   \[
   V_{\text{out}} = V_{\text{off}} + K_s \times I
   \]

   pour obtenir le courant \( I \) :

   \[
   I = \frac{V_{\text{out}} - V_{\text{off}}}{K_s}
   \]

## Mesure de vitesse

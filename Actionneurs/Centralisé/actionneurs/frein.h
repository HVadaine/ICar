#ifndef frein_h
#define frein_h

#include <Arduino.h>
#include "DualMC33926MotorShield.h"
class frein
{
  public:  
    
    // PUBLIC METHODS
    void init(); // Initialise les paramètres liés au frein
    void commande(int value); // met à jour la valeur de freinage.
	int asservissement(int pos_consigne, int pos_reel);
    
  private:
	const int ledPin = 6;
	const int Vpression = A5;
	const int  AU_pin = A0;
	const int Vpos = A3;
	const int Pos_vcc = 4;
	DualMC33926MotorShield md;


	int erreur_precP = 0;
	int erreur_sommeP = 0;
	float kp = 20;           // Coefficient proportionnel
	float ki = 0;           // Coefficient intégrateur
	float kd = 0;           // Coefficient dérivateur
};

#endif

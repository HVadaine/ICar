#ifndef variateur_h
#define variateur_h

#include <Arduino.h>
#include "DualMC33926MotorShield.h"
class variateur
{
  public:  
    
    // PUBLIC METHODS
    void init(); // Initialise les paramètres liés au frein
    void commande(int value); // met à jour la valeur de freinage.
	int asservissement(int pos_consigne, int pos_reel);
	void lock();
	void initvar();
    
  private:
	const int CapteurV = A0;
	const int alim_therm=12;
	const int dem_therm=11;
	const int Vitesse = 0;
	const int embrayage_plus =8;
	const int emb=0;
	const int embrayage_min =3;
	const int mass_pin=4;
	const int vacc_pin=6;
	const int FS_pin=7;

  boolean isInit =false;
	int erreur_precP = 0;
	int erreur_sommeP = 0;
	float kp = 20;           // Coefficient proportionnel
	float ki = 0;           // Coefficient intégrateur
	float kd = 0;           // Coefficient dérivateur
};

#endif

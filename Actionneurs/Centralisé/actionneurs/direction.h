#ifndef direction_h
#define direction_h

#include <Arduino.h>
class direction
{
  public:  
    
    // PUBLIC METHODS
    void init(); // Initialise les paramètres liés au frein
    void commande(int value); // met à jour la valeur de freinage.
	int asservissement(int pos_consigne, int pos_reel);
	void lock();
	void unlock();

    
  private:
	const int carte_de_puissance =10;
	const int alim_potar=6;
	const int retour_potar=A4;

	int erreur_precP = 0;
	int erreur_sommeP = 0;
	float kp = 20;           // Coefficient proportionnel
	float ki = 0;           // Coefficient intégrateur
	float kd = 0;           // Coefficient dérivateur
};

#endif
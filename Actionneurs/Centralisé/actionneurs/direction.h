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
	const int retour_potar=A4;

	int erreur_precP = 0;
	int erreur_sommeP = 0;

  //Coef de l'asservissement : améliorable (prendre en compte le coef de frottement en fonction de la vitesse) 
	float kp = 5;           // Coefficient proportionnel
	float ki = 0.3;           // Coefficient intégrateur
	float kd = 0;           // Coefficient dérivateur
};

#endif

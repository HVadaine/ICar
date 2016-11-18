#include "direction.h"

// Constructors ////////////////////////////////////////////////////////////////


// Public Methods //////////////////////////////////////////////////////////////
void direction::init()
{
// Define pinMode for the pins and set the frequency for timer1.
  pinMode(carte_de_puissance, OUTPUT);             // Et la sortie de la LED
  pinMode(alim_potar,OUTPUT);
  pinMode(retour_potar,INPUT);
  digitalWrite(alim_potar,HIGH);
}

// Set speed for motor 2, speed is a number betwenn -400 and 400
void direction::commande(int value)
{
	int commande_asservie=asservissement(value,analogRead(retour_potar));
  if(commande_asservie < 65) {commande_asservie = 65;} 
  else if(commande_asservie >245){commande_asservie =245;}
  analogWrite(carte_de_puissance,commande_asservie);
}

int direction::asservissement(int pos_consigne, int pos_reel){
  int commande;

   
  int erreur =  pos_reel- pos_consigne; // calcul de l'erreur nominale
  erreur_sommeP += erreur; // calcul du retard/avance sur l'erreur (si on corrige trop ou pas assez sur le long terme
  
  int erreur_delta = erreur - erreur_precP; // calcul de la vitesse de supression de l'erreur
  erreur_precP = erreur;
  commande = kp * erreur + ki * erreur_sommeP + kd * erreur_delta;	
  return commande;

}

//au démarage et lorsque que l'on envoie une commande hors bornes (65/245) le variateur se met en mode sécu et se bloque, cette méthode permet de le repasser en mode manuel
void direction::unlock()
{
  int i=0;
  while(i<40)
  {
    analogWrite(carte_de_puissance,245);
    delay(100);
    analogWrite(carte_de_puissance,65);
    delay(100);
    i++;  
  }
}
void direction::lock()
{
  analogWrite(carte_de_puissance,0); // permet de mettre le variateur rokraft en pause. ATTENTION : PLUS DE COUPLE APPLIQUE SUR LE VOLANT
}

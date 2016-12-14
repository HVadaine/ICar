#include "frein.h"

// Constructors ////////////////////////////////////////////////////////////////


// Public Methods //////////////////////////////////////////////////////////////
void frein::init()
{
// Define pinMode for the pins and set the frequency for timer1.
  pinMode(ledPin, OUTPUT);            // Et la sortie de la LED
  pinMode(13, OUTPUT);                // on met la broche qui alimente le potentiomètre en type sortie
  pinMode(AU_pin, INPUT);
  digitalWrite(13, HIGH);             // et on ma met à 1 pour sortir du 5v
  digitalWrite(Pos_vcc, HIGH);
  analogWrite(ledPin, 127);
  md.init();
}

// Set speed for motor 2, speed is a number betwenn -400 and 400
void frein::commande(int value)
{ //Serial.print("Value : "); Serial.print(value);
	int commande=asservissement(value,analogRead(Vpression));
//	Serial.print("Asservissement Frein : ");
//	Serial.print(asservissement(value,analogRead(Vpression)));
  if (commande < -400) commande = -400; //on limite l'intervalle de commande à l'intervale supporté par la carte
  else if (commande > 400) commande = 400;
  md.setM1Speed(commande);
  /*Serial.print("; commande frein : ");
  Serial.print(commande);
  Serial.print("; Capteur pression : ");
  Serial.println(analogRead(Vpression));//37 minimum, 120 max*/
}

int frein::asservissement(int pression_consigne, int pression_reel){
  int commande;
  int erreur =  (pression_reel - pression_consigne)*-1; // calcul de l'erreur nominale
  erreur_sommeP += erreur; // calcul du retard/avance sur l'erreur (si on corrige trop ou pas assez sur le long terme
  
  int erreur_delta = erreur - erreur_precP; // calcul de la vitesse de supression de l'erreur
  erreur_precP = erreur;
  commande = kp * erreur + ki * erreur_sommeP + kd * erreur_delta;	
  return commande;

}


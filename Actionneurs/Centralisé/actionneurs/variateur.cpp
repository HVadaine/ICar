#include "variateur.h"

// Constructors ////////////////////////////////////////////////////////////////


// Public Methods //////////////////////////////////////////////////////////////
void variateur::init()
{
// Define pinMode for the pins and set the frequency for timer1.
  pinMode(mass_pin, OUTPUT);
  pinMode(vacc_pin, OUTPUT);
  pinMode(FS_pin, OUTPUT);
  //pinMode(alim_therm,OUTPUT);
  //pinMode(dem_therm,OUTPUT);
  pinMode(embrayage_min,OUTPUT);
  pinMode(embrayage_plus,OUTPUT);
  digitalWrite(embrayage_min,LOW);
  digitalWrite(embrayage_plus,HIGH);
  digitalWrite(mass_pin,LOW);
  digitalWrite(FS_pin, LOW);//HIGH je pilote , low je bloque
  analogWrite(vacc_pin,0);
 initvar();
}

// Set speed for motor 2, speed is a number betwenn -400 and 400
void variateur::commande(int value)
{
  if(!isInit)
  {
    initvar();
  }
	int commande=value; //asservissement(value,analogRead(Vpos)); Pas d'asservissement pour le moment : pas de retour capteur pour la vitesse
   if(commande < 0) commande=0;
   else if(commande > 155) commande = 155;//on bride le moteur pour limiter la vitesse
   analogWrite(vacc_pin,commande);
}

int variateur::asservissement(int pos_consigne, int pos_reel){
  int commande;

   
  int erreur =  pos_reel- pos_consigne; // calcul de l'erreur nominale
  erreur_sommeP += erreur; // calcul du retard/avance sur l'erreur (si on corrige trop ou pas assez sur le long terme
  
  int erreur_delta = erreur - erreur_precP; // calcul de la vitesse de supression de l'erreur
  erreur_precP = erreur;
  commande = kp * erreur + ki * erreur_sommeP + kd * erreur_delta;	
  return commande;

}

void variateur::lock()
{
  digitalWrite(vacc_pin,0);
  digitalWrite(FS_pin,LOW);
  isInit=false;
}
void variateur::initvar()
{
    digitalWrite(FS_pin,LOW);
    digitalWrite(embrayage_plus,HIGH);
    digitalWrite(mass_pin,LOW);
    delay(5);
    digitalWrite(mass_pin,HIGH);
    delay(5);
    digitalWrite(mass_pin,LOW);
    delay(5);
    digitalWrite(mass_pin,LOW);
     digitalWrite(FS_pin,LOW);
     isInit=true;
}



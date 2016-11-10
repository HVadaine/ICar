#include "DualMC33926MotorShield.h"


int ledPin = 6;                       // Sortie où sera branchée la LED
int ledState = LOW;                    // Etat de la LED (LOW par défaut)
DualMC33926MotorShield md;
const int Vpression = A5;
int  AU_pin = A0;
const int Vpos = A3;
int Pos_vcc = 4;
int Npos;
int Npression = 0;
int Nconsigne = 0;
int pression = 0;
int consigne = 0;
int ecart = 0 ;
int verin = 0;
int pos = 0;
int sF = 0;
int v = 0;

int ech = 1;
int cmd;
int erreur_precF = 0;
int erreur_sommeF = 0;
int erreur_precP = 0;
int erreur_sommeP = 0;
float kp = 20;           // Coefficient proportionnel
float ki = 0;           // Coefficient intégrateur
float kd = 0;           // Coefficient dérivateur

void stopIfFault()
{
  if (md.getFault())
  {
    Serial.println("fault");
    while (1);
  }
}

void setup() {
  Serial.begin(57600);                  // On initialise la connexion
  pinMode(ledPin, OUTPUT);            // Et la sortie de la LED
  pinMode(13, OUTPUT);                // on met la broche qui alimente le potentiomètre en type sortie
  pinMode(AU_pin, INPUT);
  digitalWrite(13, HIGH);             // et on ma met à 1 pour sortir du 5v
  digitalWrite(Pos_vcc, HIGH);
  analogWrite(ledPin, 127);
  md.init();
}

void loop() {
  sF = ConsigneF(sF);
  Npos = analogRead(Vpression) - 33;
  if (sF < 50) {
    analogWrite(ledPin, 0);
  }
  else {
    analogWrite(ledPin, 255);
  }
  if (analogRead(AU_pin) < 50) {
    if (sF <= 0) {
      asservissementP(560, analogRead(Vpos));
    }
    else
    {
      asservissementF(sF, Npos);
    }
  }
  else
  {
    asservissementF(400, Npos);
  }
}
int ConsigneF(int memo) {
  int k, received;
  if (Serial.available() > 0) {        // Si des données sont disponibles
    char c[2];
    int d, u;
    Serial.readBytes(c, 2);
    d = c[0] - 48;
    u = c[1] - 48;
    k = (10 * d + u) * 10;
    Npos = analogRead(Vpression); // -1 à 24 Bar mesurable sur 0 à 5V. 5V discrétisés en 1023 valeur : 1 <=>  4.9mV <=> 0.025 Bar
    Serial.println(Npos);  // verin sorti : 35=0.875 Bar, verin rentré : 450=11.25 Bar, on veut freiner au max a 8 Bar <=> 320.
    //Serial.println(analogRead(Vpos));
    return k;
  }
  else {
    return memo;
  }

}
void asservissementF(int pos_consigne, int pos_reel) {
  int commande;
  if (pos_consigne > 450) {
    pos_consigne = 450;
  }
  int erreur = pos_consigne - pos_reel;
  erreur_sommeF += erreur;
  int erreur_delta = erreur - erreur_precF;
  erreur_precF = erreur;

  commande = kp * erreur + ki * erreur_sommeF + kd * erreur_delta;
  if (commande < -400) commande = -400;
  else if (commande > 400) commande = 400;
  md.setM1Speed(commande);
  delay(5);

}

void asservissementP(int pos_consigne, int pos_reel) {
  int commande;
  pos_consigne = -pos_consigne;
  pos_reel = -pos_reel;
  int erreur = pos_consigne - pos_reel;
  erreur_sommeP += erreur;
  int erreur_delta = erreur - erreur_precP;
  erreur_precP = erreur;
  commande = 5 * erreur + 0 * erreur_sommeP + 1 * erreur_delta;
  //Serial.println(commande);
  if (commande < -400) commande = -400;
  else if (commande > 400) commande = 400;
  md.setM1Speed(commande);
  delay(5);

}

int consigne=0;                       //Initialisation de la consigne à 0 (frein relaché)
int variateur =10;
int alim_potar=6;
int retour_potar=A4;
int val=0;
int erreur_somme=0;
int erreur_prec=0;
int kp=30;
int ki=0;
int kd=0;


void setup() {
  Serial.begin(57600);                  // On initialise la connexion
  pinMode(variateur, OUTPUT);             // Et la sortie de la LED
  pinMode(alim_potar,OUTPUT);
  pinMode(retour_potar,INPUT);
  digitalWrite(alim_potar,HIGH);
}

void loop() {
       consigne=Consigne(consigne);
       if(consigne==0){
        lock();
       }
       else if(consigne==10)
       {
        unlock();
       }
       else
       {
       asservissement(consigne,analogRead(retour_potar));
       }
       
      
}


int Consigne(int memo){
  int k,received;
  if (Serial.available()>0) {          // Si des données sont disponibles
          // On les récupère
          char c[2];
          Serial.readBytes(c,2);
          k=((c[0]-48)*10+c[1]-48)*10;
          val =analogRead(retour_potar); // min 140 gauche ; max 1000 droite ; millieu 650;
          Serial.println(val);
          return k;
          }
          else {return memo;}         //Si pas de données reçues, on garde la consigne précédente
  
}

void asservissement(int consigne, int retour_capteur)
{
  int erreur=consigne-retour_capteur;
  erreur_somme=erreur_somme+erreur;
  int erreur_delta=erreur_prec-erreur;
  erreur_prec=erreur;

  int commande=erreur*kp+erreur_somme*ki+erreur_delta*kd;
//on borne la commande pour rester dans l'intervalle de la PWM supporté par la carte de puissance rokraft
  if(commande < 65) {commande = 65;} 
  else if(commande >245){commande =245;}
  analogWrite(10,commande);
}

//au démarage et lorsque que l'on envoie une commande hors bornes (65/245) le variateur se met en mode sécu et se bloque, cette méthode permet de le repasser en mode manuel
void unlock()
{
  int i=0;
  while(i<40)
  {
    analogWrite(10,245);
    delay(100);
    analogWrite(10,65);
    delay(100);
    i++;  
  }
}
void lock()
{
  analogWrite(10,0); // permet de mettre le variateur rokraft en pause. ATTENTION : PLUS DE COUPLE APPLIQUE SUR LE VOLANT
}


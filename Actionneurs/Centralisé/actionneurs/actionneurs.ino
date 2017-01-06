#include "DualMC33926MotorShield.h"
#include "frein.h"
#include "direction.h"
#include "variateur.h"

direction dir;
variateur var;
frein fr;
String inputString = "";

 /**
  * ces variables serviront à stocker l'ordre de l'utilisateur, 
  * elles pourraient être stockées directement dans les classes associés 
  * mais je préfère séparer la partie actionneur(les .cpp) de la partie décision (le .ino)
  */
int commande_frein,commande_variateur,commande_direction;


void parseMessage();
void initTimer1();
void arret();

void setup() {
  Serial.begin(57600);
  commande_direction=500;
   dir.init();
   var.init();
   fr.init();
   initTimer1();
}

void loop() {
  

}


/**
 * Cette méthode lit bit à bit les messages 
 * puis effectue l'interpretation du message.
 */
 void serialEvent() {
  Serial.println("event detected");
  while (Serial.available()) {
    Serial.println("event read");
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      parseMessage(); 
    }
  }
}

void initTimer1()
{
  // initialiser le timer1
  noInterrupts(); // désactiver toutes les interruptions
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  OCR1A = 31250; // 16MHz/256/2Hz
  TCCR1B |= (1 << WGM12); // CTC mode
  TCCR1B |= (1 << CS12); // 256 prescaler
  TIMSK1 |= (1 << OCIE1A); // Activer le mode de comparaison
  interrupts(); // activer toutes les interruptions
}

ISR(TIMER1_COMPA_vect) // fonction périodique
{
 fr.commande(commande_frein);
 dir.commande(commande_direction);
 var.commande(commande_variateur);
}

/**
 * Cette méthode interprete le message 
 * puis enclenche l'action associée au message si il y en a une.
 * 
 * Les messages respectent une syntaxe précise :
 * - un header de 1 caractère : il défini l'action (cela laisse un peu plus de 64 000 actions programmable en théorie).
 * - une valeur  codée sur 1 caractère  : la valeur est définie sur un interval allant de 0 a 100 (0 a 100% de la commande), 
 *  en cas de dépassement d'interval l'arrêt d'urgence et la mise en défaut sont activés.
 * 
 * Si un message ne respecte pas la syntaxe ou si aucun ordre lui est associé au header la voiture entrera en arret d'urgence et en défaut 
 */
void parseMessage()
{
  Serial.println(inputString);
  switch(inputString[0])
  {
        case 'D': //Commande de la direction
        // mise à l'échelle pour que la commande de l'utilisateur corresponde à l'interval de commande de la carte de puissance
        dir.commande(inputString[1]*1.7+65);
        commande_direction=inputString[1]*1.7+65;
    Serial.println("direction");
    break;
        case 'F': //Commande du frein
        fr.commande(inputString[1]);
        commande_frein=inputString[1];
    Serial.println("frein");
    break;
        case 'V': //Commande du variateur
        var.commande(inputString[1]);
        commande_variateur=inputString[1];
    Serial.println("variateur");
    break;
        case 'S': //Commande d'arret
        arret();
    Serial.println("arret");
    break;
    case 'R'://Repos on range le frein pour eviter une usure
         fr.commande(-400);
        commande_frein=-400;
    Serial.println("repos");
    break;
    default :
        var.lock();//on met le variateur en defaut pour couper toute la puissance le plus rapidement possible
         arret();
    Serial.println("commande inconnue");
    break;
  }
  inputString = ""; //le message est consommé, donc detruit
}

void arret()
{
    dir.commande(50);
    commande_direction=50;
    fr.commande(100);
    commande_frein=100;
    var.commande(0);
    commande_variateur=0;
}


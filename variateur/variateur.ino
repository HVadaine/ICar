const int CapteurV = A0;
int alim_therm=12;
int dem_therm=11;
int Vitesse = 0;
int embrayage_plus =8;
int emb=0;
int embrayage_min =3;
int mass_pin=4;
int vacc_pin=6;
int FS_pin=7;
int potar;
int vitesse;
int consigneD=0;
int consigneV=0;
int erreur_sommeD=0;
int erreur_sommeV=0;
int erreur_precD=0;
int erreur_precV=0;
int cmdD=0;
int cmdV=0;
float kpV=0.5;
int kiV=0;
int kdV=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
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
  consigneV=0;
}

void loop() {
 
  consigneV=USB(consigneV);
 asservissement(consigneV);
 
 }

int USB(int memo){
  int k;
  if (Serial.available()>0) {// Si des données sont disponibles
          char c[13];
          int d,u,h;
          Serial.readBytes(c,13);
          if((c[0]=='F') & (c[1]=='S'))
          {
           
            if(c[2]=='1')
            {
               Serial.println("FS1");
              digitalWrite(FS_pin,LOW);
            }
            else
            {
              digitalWrite(FS_pin,HIGH);
               Serial.println("FS0");
            }
          }
          if((c[3]=='E') & (c[4]=='M'))
          {
            
            if(c[5]=='1')
            {
              Serial.println("EM1");
              digitalWrite(embrayage_plus,LOW);
            }
            else
            {
              Serial.println("EM0");
              digitalWrite(embrayage_plus,HIGH);
            }
          }
         if((c[6]=='M') & (c[7]=='A'))
          {
            
            if(c[8]=='1')
            {
              Serial.println("MA1");
              digitalWrite(mass_pin,LOW);
            }
            else
            {
              Serial.println("MA0");
              digitalWrite(mass_pin,HIGH);
            }
          }
      if((c[9]=='C') & (c[10]=='D'))
      {
          Serial.println("CD");
          k=((c[11]-48)*10+c[12]-48);
          Serial.println(c[11]);
          Serial.println(c[12]);
      }
      else
      {
        Serial.println("fail");
        k=0;
      }
          /* if(c[0]=='T')
          {
            if(c[1]=='1')
            {
              digitalWrite(alim_therm,HIGH);
              digitalWrite(dem_therm,HIGH);
              delay(3000);
              digitalWrite(dem_therm,LOW);
            }
          }*/
          if((c[0]=='i') &(c[1]=='n') &(c[2]=='i') &(c[3]=='t'))
          {
            Serial.println("init");
            initvar();
            k=0; 
          }
          if((c[0]=='L'))
          {
            Serial.println("lock");
            lock();
            k=0;
          }
          Serial.println(k);
          return k;
          }
          else {return memo;}
  
}

void asservissement(int pos_consigne){
   int erreur = pos_consigne;
   erreur_sommeV +=erreur;
   int erreur_delta=erreur-erreur_precV;
   erreur_precV=erreur;

   cmdV=kpV*erreur+kiV*erreur_sommeV+kdV*erreur_delta;
   if(cmdV < 0) cmdV=0;
   else if(cmdV > 155) cmdV = 55;
   analogWrite(vacc_pin,cmdV);
   delay(50);
    
    }

void lock()
{
  digitalWrite(vacc_pin,0);
  digitalWrite(FS_pin,LOW);
}
void initvar()
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
}


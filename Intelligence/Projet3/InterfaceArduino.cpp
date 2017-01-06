#include "InterfaceArduino.h"

InterfaceArduino::InterfaceArduino()
{
	actionneurs = gcnew System::IO::Ports::SerialPort();
	carteRazor = gcnew System::IO::Ports::SerialPort();

	actionneurs->PortName = "COM4"; //TODO : chercher une m�tode pour nommer les ports de fa�on automatique
	actionneurs->BaudRate = 57200;
	actionneurs->Open();
	
	/*
	carteRazor->PortName = "COM2";
	carteRazor->BaudRate = 57200;
	carteRazor->Open();
	carteRazor->DiscardOutBuffer(); // tr�s tr�s important cela permet d'empecher la cr�ation d'une file d'attente dans le buffer de sortie en direction du port USB, cela �vite les probl�mes de d�synchronisation li�s � des restes dans la m�moire du pc
	carteRazor->Write("#osct");
	carteRazor->Write("#o0"); // on envoie � la carte RAZOR l'odre de n'emettre que sur demande (#o0) et d'envoyer toutes les donn�es des capteurs (#osct) : pour plus d'info voir le document RAZOR_DOC.txt (ouvrez le avec notepad sinon la lecture sera difficile)
	carteRazor->DiscardInBuffer(); // on purge les entr�s pour empecher une future d�syncronisation dans la lecture des donn�s.*/
}

void InterfaceArduino::commandeActionneur(int header, int value)
{
	cout << header;
	cout << " ";
	cout << value;
	cout << "\n";
	actionneurs->Write(header.ToString());
	actionneurs->Write(value.ToString());
	actionneurs->Write("\n");
}

/**
params : System::IO::Ports::SerialPort^  serialPort : port USB sur lequel vous souhaitez �crire
String^ commande : le message que vous voulez envoyer
**/
void arduino_drive(System::IO::Ports::SerialPort^  serialPort, String^ commande)
{
		serialPort->Write(commande);
}


/**
params : System::IO::Ports::SerialPort^ serialPort : il s'agit du port USB que vous voulez lire
taille : taille estim�e du message, la taille est limit� � 1000
description : la fonction permet de lire un message venant d'un port USB.
Attention la m�thode est bloquante veuillez vous assurer qu'un message est � recevoir et qu'il se termine par un retour ligne : \n
**/
void arduino_print_return(System::IO::Ports::SerialPort^  serialPort, int taille)
{
	char byte = 'a'; //on l'initialise histoire que le test se passe bien
	char arduino_msg[1000]; // on stockera le message dans cette chaine
	int i = 0;
	while (byte != '\n' && i<taille) // on lit les messages tant que l'on ne tombe pas du un retour a la ligne (on utilise println dans la communication arduino)
	{
		byte = serialPort->ReadByte(); // on lit bit � bit, c'est une fonction robuste et simple � utiliser cela permet de ne pas trop complexifier le programme
		arduino_msg[i] = byte;//on enregistre chaque bit dans une chaine de caract�re (on est en C les string sont des char[] ne pas confondre avec java)
	}
}

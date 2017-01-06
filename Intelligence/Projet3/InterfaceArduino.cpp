#include "InterfaceArduino.h"

InterfaceArduino::InterfaceArduino()
{
	actionneurs = gcnew System::IO::Ports::SerialPort();
	carteRazor = gcnew System::IO::Ports::SerialPort();

	actionneurs->PortName = "COM4"; //TODO : chercher une métode pour nommer les ports de façon automatique
	actionneurs->BaudRate = 57200;
	actionneurs->Open();
	
	/*
	carteRazor->PortName = "COM2";
	carteRazor->BaudRate = 57200;
	carteRazor->Open();
	carteRazor->DiscardOutBuffer(); // très très important cela permet d'empecher la création d'une file d'attente dans le buffer de sortie en direction du port USB, cela évite les problèmes de désynchronisation liés à des restes dans la mémoire du pc
	carteRazor->Write("#osct");
	carteRazor->Write("#o0"); // on envoie à la carte RAZOR l'odre de n'emettre que sur demande (#o0) et d'envoyer toutes les données des capteurs (#osct) : pour plus d'info voir le document RAZOR_DOC.txt (ouvrez le avec notepad sinon la lecture sera difficile)
	carteRazor->DiscardInBuffer(); // on purge les entrés pour empecher une future désyncronisation dans la lecture des donnés.*/
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
params : System::IO::Ports::SerialPort^  serialPort : port USB sur lequel vous souhaitez écrire
String^ commande : le message que vous voulez envoyer
**/
void arduino_drive(System::IO::Ports::SerialPort^  serialPort, String^ commande)
{
		serialPort->Write(commande);
}


/**
params : System::IO::Ports::SerialPort^ serialPort : il s'agit du port USB que vous voulez lire
taille : taille estimée du message, la taille est limité à 1000
description : la fonction permet de lire un message venant d'un port USB.
Attention la méthode est bloquante veuillez vous assurer qu'un message est à recevoir et qu'il se termine par un retour ligne : \n
**/
void arduino_print_return(System::IO::Ports::SerialPort^  serialPort, int taille)
{
	char byte = 'a'; //on l'initialise histoire que le test se passe bien
	char arduino_msg[1000]; // on stockera le message dans cette chaine
	int i = 0;
	while (byte != '\n' && i<taille) // on lit les messages tant que l'on ne tombe pas du un retour a la ligne (on utilise println dans la communication arduino)
	{
		byte = serialPort->ReadByte(); // on lit bit à bit, c'est une fonction robuste et simple à utiliser cela permet de ne pas trop complexifier le programme
		arduino_msg[i] = byte;//on enregistre chaque bit dans une chaine de caractère (on est en C les string sont des char[] ne pas confondre avec java)
	}
}

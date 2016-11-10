#include <winsock2.h>
#include <iostream>
#include <fstream>      // std::filebuf
#include <string>
#include "libusb/libusb.h"
#pragma comment(lib, "ws2_32.lib")
#pragma once

using namespace std;
using namespace System;
#using <System.dll>

using namespace System::IO::Ports;
using namespace System::Threading;
void get_razor_data(struct attitude * icar_attitude, System::IO::Ports::SerialPort^  serialPort2);
void parse_razor_all_axis(char Accel[], double *data_x, double *data_y, double *data_z);
double parse_razor_one_axis(char data_in[], int cnt, int number_length);

	struct attitude
	{
		double accel_x;
		double accel_y;
		double accel_z;
		double magn_x;
		double magn_y;
		double magn_z;
		double gyro_x;
		double gyro_y;
		double gyro_z;
	};

int main()
{

	// on cr�e les variables n�cessaires au fonctionnement du programme
	struct attitude icar_attitude;
	WSADATA WSAData;
	SOCKET sock;
	SOCKET csock;
	SOCKADDR_IN sin;
	SOCKADDR_IN csin;
	bool ConnectOrder = TRUE;
	bool EndOfCom = TRUE;
	char buff[20];
	
	
	// on initialise le socket pour le serveur
	////////////////////////////////////////////////////////////////////////////////////////////
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(6001);
	bind(sock, (SOCKADDR *)&sin, sizeof(sin));
	listen(sock, 1);
	///////////////////////////////////////////////////////////////////////////////////

	
	// on initialise les ports USB pilotant les arduinos
	/////////////////////////////////////////////////////////////////////////////////////
	System::IO::Ports::SerialPort^  serialPort1 = gcnew System::IO::Ports::SerialPort();
	System::IO::Ports::SerialPort^  serialPort2 = gcnew System::IO::Ports::SerialPort();
	System::IO::Ports::SerialPort^  serialPort3 = gcnew System::IO::Ports::SerialPort();
	System::IO::Ports::SerialPort^  serialPort4 = gcnew System::IO::Ports::SerialPort();
	serialPort1->PortName = "COM1";
	serialPort1->Open();
	
	serialPort2->PortName = "COM2";
	serialPort2->Open();
	
	serialPort3->PortName = "COM3";
	serialPort3->Open();
	
	serialPort4->PortName = "COM4";
	serialPort4->BaudRate = 57200;
	serialPort4->Open();
	serialPort4->DiscardOutBuffer(); // tr�s tr�s important cela permet d'empecher la cr�ation d'une file d'attente dans le buffer de sortie en direction du port USB, cela �vite les probl�mes de d�synchronisation li�s � des restes dans la m�moire du pc
	serialPort4->Write("#osct");
	serialPort4->Write("#o0"); // on envoie � la carte RAZOR l'odre de n'emettre que sur demande (#o0) et d'envoyer toutes les donn�es des capteurs (#osct) : pour plus d'info voir le document RAZOR_DOC.txt (ouvrez le avec notepad sinon la lecture sera difficile)
	serialPort4->DiscardInBuffer(); // on purge les entr�s pour empecher une future d�syncronisation dans la lecture des donn�s.
	/////////////////////////////////////////////////////////////////////////////////////////
	
	
	
	//Boucle (infinie) g�rant la communication entre l'IHM et le PC
	while (EndOfCom)
	{
		cout << "\ntentative de connexion\n";
		if (ConnectOrder)
		{
			int sinsize = sizeof(csin);
			if ((csock = accept(sock, (SOCKADDR *)&csin, &sinsize)) != INVALID_SOCKET)// on s'est connect� � partir de l'IHM
			{
				cout << "connexion reussie\n";
				while (1) 
				{
					recv(csock, buff,80, 0); //reception des messages, !!D�finir un TIMEOUT
					 cout << buff;
					 int i = 0;
					//le premier (et pour l'instant le seul) type de message est de la forme Frein[valeur]Dir[valeur]Var[valeur]
					//le second est stop il int�rompt le programme.
					if (buff[0]=='F' & buff[1] =='r' & buff[2] == 'e' & buff[3] == 'i' & buff[4] == 'n')
					{
						cout << "\nfrein recu\n";
						char num[2];
						num[0] = buff[5];
						num[1] = buff[6];
						arduino_drive(serialPort1, num, &i);
						
						cout << "\n";

					}
					if (buff[i+6]=='D' & buff[i+7] =='i' & buff[i+8] == 'r')
					{
						cout << "\ndir recue\n";
						char num[2];
						num[0] = buff[i+9];
						num[1] = buff[i+10];
						arduino_drive(serialPort2, num, &i);
						cout << "\n";
					}
					if (buff[i+10]=='V' & buff[i+11] =='a' & buff[i+12] == 'r')
					{
						cout << "\nvar recue\n";
						char num[2];
						num[0] = buff[i+13];
						num[1] = buff[i+14];
						arduino_drive(serialPort4, num, &i);
						cout << "\n";
					}
					if (buff[0] == 's' & buff[1] == 't' & buff[2] == 'o' & buff[3] == 'p')
					{
						EndOfCom = FALSE;
						break;
					}
				}
				send(csock, "Hello world!\r\n", 14, 0);
				closesocket(csock);
			}
			ConnectOrder = FALSE;
		}
	}
	//serialPort1->Close();
	closesocket(sock);
	WSACleanup();
	return 0;
}
/**
params : char data_in[] : donn�es � traiter.
		 int ctn : emplacement du point
		 int number_length : taille du nombre
description : on transforme le char que l'on � localis� dans la chaine en double
*/
double parse_razor_one_axis(char data_in[], int cnt, int number_length)
{
	double data_out; // on cr�e la variable de retour
	//c'est pas compliqu� mais long � expliquer mais je vais r�sum� les principaux point :
	//le premier nombre peut avoir entre 1 et 3 chiffre avant la virgule et toujour 2 apr�s la virgule. Il peut �tre n�gatif.
	//le switch case g�re la taille et le if g�re l'aspect n�gatif. le cas ou il y a 4 chiffres  avant la virgule est simple le 1er caract�re est -.
	// number_length �quivaut � la taille avant la virgule +1 car le point est compris dans le d�compte.
	//les char sont cod�s en ASCII donc pour avoir les chiffres de 0 a 9 il faut soustraire 48.
	switch (number_length) {
	case 5:
		data_out = -((data_in[cnt - 3] - 48) * 100 + (data_in[cnt - 2] - 48) * 10 + data_in[cnt - 1] - 48 + (data_in[cnt + 1] - 48)*0.1 + (data_in[cnt + 2] - 48)*0.01);
		break;
	case 4:
		if (data_in[cnt - 3] == '-') 
		{
			data_out = -((data_in[cnt - 2] - 48) * 10 + data_in[cnt - 1] - 48 + (data_in[cnt + 1] - 48)*0.1 + (data_in[cnt + 2] - 48)*0.01); 
		}
		else 
		{
			data_out = (data_in[cnt - 3] - 48) * 100 + (data_in[cnt - 2] - 48) * 10 + data_in[cnt - 1] - 48 + (data_in[cnt + 1] - 48)*0.1 + (data_in[cnt + 2] - 48)*0.01;
		}
		break;
	case 3:
		if (data_in[cnt - 2] == '-') 
		{
			data_out = -(data_in[cnt - 1] - 48 + (data_in[cnt + 1] - 48)*0.1 + (data_in[cnt + 2] - 48)*0.01); 
		}
		else
		{
			data_out = (data_in[cnt - 2] - 48) * 10 + data_in[cnt - 1] - 48 + (data_in[cnt + 1] - 48)*0.1 + (data_in[cnt + 2] - 48)*0.01;
		}
		break;
	default:
		data_out = data_in[cnt - 1] - 48 + (data_in[cnt + 1] - 48)*0.1 + (data_in[cnt + 2] - 48)*0.01;
		break;
	}
	return data_out;

}
/**
params : char data[] : la variable qui contient les donn�es � traiter : Accel, Magn ou Gyro
			data_* : variable de stockage des r�sultat : accel_*, magn_* ou gyro_*

description : on d�coupe encore le message en trois morceau mais cette fois ci suivant les 3 axes.
*/
void parse_razor_all_axis(char data[], double *data_x, double *data_y, double *data_z) 
{
	int number_length = 0; // la taille des nombres n'est pas fixe il faut le g�rer
	int x_parse = 0, y_parse = 0, z_parse = 0; // variable d'�tat pour savoir sur quelle partie on travaille
	for (int cnt = 0; cnt < 25; cnt++) 
	{

		if (x_parse == 1 || y_parse == 1 || z_parse == 1)
		{
			number_length++;  // si on travaille (on se trouve dans une partie avec un nombre � traiter) on incr�mente number_length pour d�terminer la taille du nombre
		}
		if (data[cnt] == '=') 
		{
			x_parse = 1;   // si on passe le signe = on rentre dans la partie contenant de l'information
	    }
		if (z_parse == 1 && data[cnt] == '.') 
		{
			*data_z = parse_razor_one_axis(data, cnt, number_length); //on transforme le char que l'on a localis� en double
			z_parse = 0; // on met l'�tat � jour
		}

		if (y_parse == 1 && data[cnt] == '.') 
		{
			*data_y = parse_razor_one_axis(data, cnt, number_length);//on transforme le char que l'on a localis� en double
			y_parse = 0;// on met l'�tat � jour
			z_parse = 1;// on met l'�tat � jour
			number_length = -3;//on c'est arret� au point, apr�s il y aura toujour 2 chiffres + 1 virgule, on remet le compteur � z�ro puis on d�cale de 3.
		}

		if (x_parse == 1 && data[cnt] == '.') 
		{
			*data_x = parse_razor_one_axis(data, cnt, number_length);//on transforme le charq ue l'on a localis� en double
			x_parse = 0;// on met l'�tat � jour
			y_parse = 1;// on met l'�tat � jour
			number_length = -3;//on c'est arret� au point, apr�s il y aura toujour 2 chiffres + 1 virgule, on remet le compteur � z�ro puis on d�cale de 3.
		}
	}
}

/**
params : struct attitude *icar_attitude : structure dans laquelle on stocke les donn�s trait�s de la central d'attitude (la carte RAZOR IMO 9dof)
System::IO::Ports::SerialPort^  serialPort2 : PortUSB de la carte RAZOR IMO 9dof 

description : on lit, traite et stocke l'attitude(acceleration, position angulaire relative, postion angulaire absolue (boussole)) de la voiture 
*/
void get_razor_data(struct attitude *icar_attitude, System::IO::Ports::SerialPort^  serialPort2)
{
	serialPort2->Write("#f"); //la carte a besoin d'une requette pour emettre (on a coup� le flux continu avec la commande #o0) #f force la carte a envoyer l'attitude
	char Accel[30], Magn[30], Gyro[30]; // variable de stockage interm�diaire on d�coupe le message en 3;
	double accel_x = 0, accel_y = 0, accel_z = 0, magn_x = 0, magn_y = 0, magn_z = 0, gyro_x = 0, gyro_y = 0, gyro_z = 0; // les variable de stockage pour la fonction, je pourrai passer directement par la structure mais cette m�thode est plus simple
	for (int cnt = 0; cnt < 25; cnt++)  // on lit bit � bit la variable d'acceleration et on la stocke
	{
		char razor = serialPort2->ReadByte();
		cout << razor; Accel[cnt] = razor;
		if (razor == '\n') { break; }
	}
	for (int cnt = 0; cnt < 25; cnt++)// on lit bit � bit la variable d'angle absolue et on la stocke
	{
		char razor = serialPort2->ReadByte();
		cout << razor; Magn[cnt] = razor; 
		if (razor == '\n') { break; }
	}
	for (int cnt = 0; cnt < 25; cnt++) // on lit bit � bit la variable d'angle relatif et on la stocke
	{
		char razor = serialPort2->ReadByte();
		cout << razor; 
		Gyro[cnt] = razor; 
		if (razor == '\n') { break; }
	}
	cout << '\n';
	parse_razor_all_axis(Accel, &accel_x, &accel_y, &accel_z); // on traite les donn�s afin d'obtenir des double qui seront utilisables.
	cout << accel_x;
	cout << '\n';
	cout << accel_y;
	cout << '\n';
	cout << accel_z;
	cout << '\n';
	parse_razor_all_axis(Magn, &magn_x, &magn_y, &magn_z); // on traite les donn�s afin d'obtenir des double qui seront utilisables.
	cout << magn_x;
	cout << '\n';
	cout << magn_y;
	cout << '\n';
	cout << magn_z;
	cout << '\n';
	parse_razor_all_axis(Gyro, &gyro_x, &gyro_y, &gyro_z); // on traite les donn�s afin d'obtenir des double qui seront utilisables.
	cout << gyro_x;
	cout << '\n';
	cout << gyro_y;
	cout << '\n';
	cout << gyro_z;
	//on stocke tout dans la structure
	icar_attitude->accel_x = accel_x; 
	icar_attitude->accel_y = accel_y;
	icar_attitude->accel_z = accel_z;
	icar_attitude->magn_x = magn_x;
	icar_attitude->magn_y = magn_y;
	icar_attitude->magn_z = magn_z;
	icar_attitude->gyro_x = gyro_x;
	icar_attitude->gyro_y = gyro_y;
	icar_attitude->gyro_z = gyro_z;
}

/**
params : System::IO::Ports::SerialPort^  serialPort : port USB sur lequel vous souhaitez �crire
		char commande[] : le message que vous voulez envoyer (la plupart du temps c'est pour commander un actionneur
		int *i  : un pointeur pour le compteur de lecture de message, si vous utiliser la fonction dans un autre contexte mettez NULL
		
		METHODE A GENERALISER
**/
void arduino_drive(System::IO::Ports::SerialPort^  serialPort, char commande[],int *i) 
{
	if (commande[1] == 'D' || commande[1] == 'V' || commande[1] == ' ') 
	{
		cout << commande[0];
		serialPort->Write("00");
	}
	else 
	{
		cout << (commande[0] - 48) * 10 + commande[1] - 48; *i++;
		serialPort->Write((commande[0] - 48).ToString() + (commande[1] - 48).ToString());
	}

}


/**
params : System::IO::Ports::SerialPort^ serialPort : il s'agit du port USB que vous voulez lire
		 taille : taille estim�e du message, la taille est limit� � 1000
description : la fonction permet de lire un message venant d'un port USB.
			  Attention la m�thode est bloquante veuillez vous assurer qu'un message est � recevoir et qu'il se termine par un retour ligne : \n
**/
void arduino_print_return(System::IO::Ports::SerialPort^  serialPort, int taille)
{
	char byte='a'; //on l'initialise histoire que le test se passe bien
	char arduino_msg[1000]; // on stockera le message dans cette chaine
	int i=0;
	while (byte != '\n' && i<taille) // on lit les messages tant que l'on ne tombe pas du un retour a la ligne (on utilise println dans la communication arduino)
	{
		byte=serialPort->ReadByte(); // on lit bit � bit, c'est une fonction robuste et simple � utiliser cela permet de ne pas trop complexifier le programme
		arduino_msg[i] = byte;//on enregistre chaque bit dans une chaine de caract�re (on est en C les string sont des char[] ne pas confondre avec java)
	}
}
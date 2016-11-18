#include "Razor.h"

/**
params : char data_in[] : donn�es � traiter.
int ctn : emplacement du point
int number_length : taille du nombre
description : on transforme le char que l'on � localis� dans la chaine en double
*/
double Razor::parse_razor_one_axis(char data_in[], int cnt, int number_length)
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
void Razor::parse_razor_all_axis(char data[], double *data_x, double *data_y, double *data_z)
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
void Razor::get_razor_data(struct attitude *icar_attitude, System::IO::Ports::SerialPort^  serialPort2)
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

attitude Razor::getAttitude()
{
	return icar_attitude;
}
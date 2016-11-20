#ifndef interfaceArduino_h
#define interfaceArduino_h
#include <iostream>
#include <fstream>      // std::filebuf
#include <string>


	using namespace std;
	using namespace System;
#using <System.dll>
	using namespace System::IO::Ports;

	void arduino_drive(System::IO::Ports::SerialPort^  serialPort, String^ commande);
	void arduino_print_return(System::IO::Ports::SerialPort^  serialPort, int taille);

	ref class InterfaceArduino
	{
	public:
		 InterfaceArduino();
		 void commandeActionneur(int header, int value);
		 int FREIN = 'F';
		 int DIRECTION = 'D';
		 int VARIATEUR = 'V';
		 

	private:
		System::IO::Ports::SerialPort^ actionneurs;
		System::IO::Ports::SerialPort^ carteRazor;
	};

#endif
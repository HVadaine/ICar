#pragma once
#ifndef interfaceArduino_h
#define interfaceArduino_h
#include <iostream>
#include <fstream>      // std::filebuf
#include <string>


using namespace std;
using namespace System;
#using <System.dll>
using namespace System::IO::Ports;

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



class Razor
{
public:
	void get_razor_data(struct attitude * icar_attitude, System::IO::Ports::SerialPort^  serialPort2);
	void parse_razor_all_axis(char Accel[], double *data_x, double *data_y, double *data_z);
	double parse_razor_one_axis(char data_in[], int cnt, int number_length);
	attitude getAttitude();
private:
	struct attitude icar_attitude;
};
#endif
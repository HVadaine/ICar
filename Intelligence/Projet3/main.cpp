#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#include "InterfaceArduino.h"

WSADATA WSAData;
SOCKADDR_IN sock_in;
SOCKADDR_IN csock_in;
char buff[2];
SOCKET sock;
SOCKET csock;

int main()
{
	InterfaceArduino^ IUA= gcnew InterfaceArduino(); //la classe arduino est "managée" cela lui donne accès a diverses fonctionnalités notement le contrôle des ports usb.

	WSAStartup(MAKEWORD(2, 0), &WSAData);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
	sock_in.sin_family = AF_INET;
	sock_in.sin_port = htons(6001);
	bind(sock, (SOCKADDR *)&sock_in, sizeof(sock_in));
	listen(sock, 1);

	int sinsize = sizeof(csock_in);
	while ((csock = accept(sock, (SOCKADDR *)&csock_in, &sinsize)) == INVALID_SOCKET)
	{


	}	
	while (1)
		{
		recv(csock, buff, 2,0);
		cout << buff[0];
		cout << " ";
		cout << buff[1];
		cout << "\n";
		if (buff[0] == 'S')
		{
			break;
		}
		IUA->commandeActionneur(buff[0], buff[1]);
		}
	return 0;
}

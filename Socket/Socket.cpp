#include "Socket.h"

/* Khoi tao winsock*/
int Socket::_initWinsock() {

	//MAKEWORD(2, 2) gui thong diep de WSAStartup deu su dung version 2.2 cua Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	//Neu khoi tao that bai 
	if (iResult) {
		printf("WSAStartup failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	return 0;
}

/* API */
int Socket::initWinsock() {
	return _initWinsock();
}
#pragma once
#ifndef _SOCKET_H_
#define _SOCKET_H_

#define _CRT_SECURE_NO_WARNINGS

#define LOCAL_PORT "8080"
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 4096

#define DATA "./stuffs/data.dat"
#define INFO "./stuffs/info.html"
#define INDEX "./stuffs/index.html"
#define ERROR404 "./stuffs/404.html"
#define FILES "./stuffs/files.html"
#define DOWNLOAD "./stuffs/download/"

#include <iomanip>
#include <fstream>
#include <string>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <sstream>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

class Socket {
public:
	Socket() {};
	int initWinsock();
private:
	//Variable
	WSADATA wsaData;
	
	//Method
	int _initWinsock();
};



class Server : public Socket {
public:
	Server(const string& portTemp) {

		//Copy tu portTemp sang port
		for (unsigned int i = 0; i < portTemp.length(); ++i) port[i] = portTemp[i];

		//Khoi tao Winshoc
		initWinsock();
	}

	int start();
private:

	//Bien
	char port[5];
	addrinfo* result = NULL;
	SOCKET listeningSocket = INVALID_SOCKET;

	//server socket method
	int createServerSocket(const char*);
	int bindSocket();
	int listenSocket();
	int acceptClient(SOCKET &);
	int shutdownSocket(SOCKET &);

	//method giao tiep giua server va client
	int communication(SOCKET &);
	string getResponse(string &, bool&, SOCKET&);
		string processGET(string&, bool&, SOCKET&);
			string gotoIndex(int);
			string gotoInfo(int);
			string gotoError();
			string gotoFile(int, const string&);
			string transferChunkedEncode(SOCKET&, string);
		string processPOST(string&, bool&);
			bool accountCheck(const string&, const string&);
	

	//method giai phong vung nho
	void releaseAll();	//Chi su dung khi listeningSocket, result deu duoc khoi tao thanh cong

	//mo rong
	string *getFile(const string& path);
};

#endif

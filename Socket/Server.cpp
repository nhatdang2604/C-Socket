#include "Socket.h"

void Server::releaseAll() {
	WSACleanup();
	closesocket(listeningSocket);
}

int Server::start() {

	int errorCode = 0;
	//Neu co loi trong qua trinh setup server socket => thoat ra
	if ((errorCode = createServerSocket(port)) != 0) return errorCode;
	if ((errorCode = bindSocket()) != 0) return errorCode;
	if ((errorCode = listenSocket()) != 0) return errorCode;
	SOCKET clientSocket;
	//Listening toi khi nao bi loi hoac khong con accept
	do {
		cout << "Waiting for new client ....\n";
		clientSocket = INVALID_SOCKET;

		//Ben trong acceptClient da giai phong bo nho neu co loi xay ra
		if ((errorCode = acceptClient(clientSocket)) != 0) break;
		
	} while (!communication(clientSocket));

	//Vi trong acceptClient(), khi co loi xay ra ta da giai phong vung nho
	//	=> De tranh giai phong vung nho 2 lan, ta can phai xet dieu kien xem acceptClient() co loi hay chua

	//Neu khong co loi acceptClient()
	if (!errorCode) return shutdownSocket(clientSocket);
	else {

		//Neu co loi acceptClient
		int iResult = shutdown(clientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			return 9;
		}
		return errorCode;
	}
}

int Server::createServerSocket(const char* port) {
	
	//Lay thong tin addr vao hints
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	
	//Server se xu li ve local address va port sap duoc su dung
	//	Thong tin lay duoc se luu ve result
	int iResult = getaddrinfo(NULL, port, &hints, &result);
	
	//Neu khong giai quyet duoc => Loi
	//	=> Phan giai WSA da cap o server
	//		=> Dung server
	if (iResult) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 2;
	}

	//Tao 1 socket de lang nghe ket noi
	listeningSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	//Neu khong tao socket thanh cong 
	//	=> Phan giai WSA da cap o server 
	//		va phan giai result (1 con tro) da duoc su dung
	//		=> Dung server
	if (listeningSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 3;
	}

	//Neu khong co loi xay ra
	return 0;
}

int Server::bindSocket() {

	//Setup socket de lang nghe ket noi
	int iResult = bind(listeningSocket, result->ai_addr, (int)result->ai_addrlen);

	//Neu viec bind socket lang nghe that bai
	//	=> Giai phong vung nho va dung Server
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		releaseAll();
		return 4;
	}

	//Neu da binding socket thanh cong, chung ta se khong can phai luu thong tin o trong result nua
	//	=> Giai phong result
	freeaddrinfo(result);

	//Neu khong co loi xay ra
	return 0;
}

int Server::listenSocket() {

	//	Bat dau lang nghe tren listeningSocket
	//voi SOMAXCONN la do dai toi da cua hang doi
	//cac ket noi dang cho xu ly
	int iResult = listen(listeningSocket, SOMAXCONN);

	//Neu viec listen that bai
	//	=> Giai phong vung nho va ket thuc chuong trinh
	if (iResult == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		releaseAll();
		return 5;
	}

	cout << "Listening in port: " << port << endl;

	//Neu khong co loi xay ra
	return 0;
}

int Server::acceptClient(SOCKET &clientSocket) {
	
	//Bat dau accept client socket
	//	Vi ta da luu lai gia tri cua result->ai_addr va result->ai_addrlen
	//		=> Khong can thiet de nhap vao accept() nua
	clientSocket = accept(listeningSocket, NULL, NULL);

	//Neu co loi xay ra => Giai phong vung nho va ket thuc chuong trinh
	if (clientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		releaseAll();
		return 6;
	}

	//Neu khong co loi xay ra
	cout << "Accepted! \n";
	return 0;
}

string* Server::getFile(const string& path) {
	ifstream file(path);

	if (file.is_open()) {
		string* buffer = new string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
		file.close();
		return buffer;
	}

	return nullptr;
}

int Server::communication(SOCKET& clientSocket) {

	//Bien co dinh khi giao tiep
	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	//Bien danh cho de bai
	string request;
	string response;
	bool loginYet = false;

	//Thuc hien cho toi khi shutdown connection
	do {
		
		//Thuc hien nhan request tu client socket
		iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
		std::cout << recvbuf << std::endl;

		//Khi van con nhan duoc request
		if (iResult > 0) {

			//Dua recvbuf ve dang string de de xu li
			request = recvbuf;
			response = getResponse(request, loginYet, clientSocket);
			
			//Gui response cho client
			iSendResult = send(clientSocket, response.c_str(), response.size(), 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(clientSocket);
				releaseAll();
				return 7;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			releaseAll();
			return 8;
		}

	} while (iResult > 0);

	return 0;
}

int Server::shutdownSocket(SOCKET& clientSocket) {
	int iResult = shutdown(clientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		releaseAll();
		return 9;
	}
	return 0;
}

string Server::getResponse(string& request, bool& loginYet, SOCKET& clientSocket) {
	
	//Neu la method GET => xu li theo GET
	if (request.find("GET") != string::npos) {
		return processGET(request, loginYet, clientSocket);
	}
	else {
		//Nguoc lai, chac chan la method POST => xu li theo POST
		return processPOST(request, loginYet);
	}
}

string Server::processGET(string& request, bool& loginYet, SOCKET& clientSocket) {
	
	//Neu truy cap vao dung index.html
	//	=>Status code: 200
	if (request.substr(sizeof("GET"), sizeof("/index.html") - 1)=="/index.html") {
		return gotoIndex(200);
	}

	//Neu truy cap vao localhost 
	if (request.substr(sizeof("GET"), sizeof("/ ") - 1) == "/ ") {

		//Neu chua dang nhap
		//	=>Status code: 301
		if (!loginYet) return gotoIndex(301);
		//Neu da dang nhap
		else return gotoInfo(301);
	}

	//Neu truy cap vao info.html
	if (request.substr(sizeof("GET"), sizeof("/info.html") - 1) == "/info.html") {
		
		//Neu chua dang nhap
		//	=> 404
		if (!loginYet) return gotoError();
		//Neu dang nhap roi
		//	=> truy cap info.html
		else return gotoInfo(200);
	}
	
	//Neu truy cap vao files.html
	if (request.substr(sizeof("GET"), sizeof("/files.html") - 1) == "/files.html") {
		 return gotoFile(200, "");
	}

	//Neu truy cap vao folder /download/
	string token = request.substr(sizeof("GET"));
	string filePath = token.substr(0, token.find(' '));
	//Chi khi tim thay "/download/" thi moi thuc hien
	if (filePath.find("/download/") != string::npos) {
		//Tai file trong /download/	=> filePath la: /download/file.xxx/
		return transferChunkedEncode(clientSocket, filePath);
	}
	else return gotoError();
	
}

string Server::transferChunkedEncode(SOCKET& clientSocket, string filePath) {
	filePath = "./stuffs" + filePath;
	FILE* file = fopen(filePath.c_str(), "rb");
	char *buffer = new char[DEFAULT_BUFLEN];
	int buffLen;
	//chieu dai cua data se duoc dua ve dang hex, luu trong hexForm
	stringstream hexForm;

	//Tien hanh gui header truoc
	string response;
	response = "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n";
	send(clientSocket, response.c_str(), response.size(), 0);

	//Bat dau gui data
	do {

		//Nhan chieu dai cua data (luu trong buffLen), va luu lai duoi dang hex nam trong hexForm
		response = "";
		buffLen = fread(buffer, 1, DEFAULT_BUFLEN, file);
		hexForm << hex << buffLen;
		response += hexForm.str() + "\r\n";
		//Clear stream
		hexForm.str("");

		//Nhan data
		response.append(buffer, buffLen);
		response += "\r\n";

		//Khi chua phai terminal chunked => tiep tuc gui
		if (buffLen > 0) {
			send(clientSocket, response.c_str(), response.size(), 0);
			printf("Bytes sent: %d\n", response.size());
		}
	} while (buffLen);
	delete[]buffer;
	fclose(file);
	//Gui lan cuoi cung la terminal chunked
	return "0\r\n\r\n";
}

string Server::gotoFile(int statusCode, const string& path) {
	string response = "HTTP/1.1 " + to_string(statusCode);

	//Neu chi moi vao files.html
	if (statusCode == 200) {
		string *script = getFile(FILES);
		response += " OK\r\nContent-Type: text/html\r\nContent-Length: " +  to_string(script->size()) + "\r\n\r\n";
		response += *script;
		delete script;
	}

	return response;
}

string Server::gotoInfo(int statusCode) {
	string response = "HTTP/1.1 " + to_string(statusCode);

	if (statusCode == 200) {
		//Di toi dung info.html va da dang nhap

		string *file = getFile(INFO);
		response += " OK\r\nContent-Type: text/html\r\nContent-Length: " + to_string(file->size()) + "\r\n\r\n";
		response += *file;

		delete file;
	}
	else {
		if (statusCode == 301) {
			//Vao local host va da dang nhap
			//	=> Redirect sang info.html
			//	=> Status code = 301
			response += " Moved permanently\r\nLocation: /info.html\r\n\r\n";
		}
		else {
			//Dang nhap thanh cong
			// =>Status code = 302
			response += " Found\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
		}
	}
	return response;
}

string Server::gotoError() {
	string *file = getFile(ERROR404);
	string response = "HTTP/1.1 404 Not found\r\nContent-Type: text/html\r\nContent-Length: " + to_string(file->size()) + "\r\n\r\n" + *file;
	delete file;
	return response;
}

string Server::gotoIndex(int statusCode) {
	string response = "HTTP/1.1 " + to_string(statusCode);

	if (statusCode == 200) {
		//Di toi dung index.html

		string *file = getFile(INDEX);
		response += " OK\r\nContent-Type: text/html\r\nContent-Length: " + to_string(file->size()) + "\r\n\r\n";
		response += *file;

		delete file;
	}
	else {
		//Nhap ip nhung chua dang nhap
		//	=> Redirect sang index.html
		//	=> status code == 301
		response += " Moved permanently\r\nLocation: /index.html\r\n\r\n";	
	}
	return response;
}

string Server::processPOST(string& request, bool& loginYet) {
	int penultimateEqualSymbolPos = request.size();

	int usernamePos = request.find("username=") + sizeof("username=") - 1;
	int ampersandPos = request.find('&');
	string username = request.substr(usernamePos, ampersandPos - usernamePos);

	//Khong can phai lay nhieu hon sizeof("admin") - 1 phan tu, vi lon hon => khong khop voi password 
	string password = request.substr(ampersandPos + sizeof("password") + 1, sizeof("admin") - 1);

	//Neu dung username va password => redirect sang Info, status code 301
	if ((loginYet = accountCheck(username, password)) ==true) {
		return gotoInfo(302);
	}
	else {
		return gotoError();
	}
}

bool Server::accountCheck(const string& username,const string& password) {
	string*data = getFile(DATA);
	if (*data == (username + "&^#@" + password)) {
		delete data;
		return true;
	}
	delete data;
	return false;
}
#include "Socket.h"

int main() {
	Server server(LOCAL_PORT);
	int errorCode = server.start();
	return 0;
}
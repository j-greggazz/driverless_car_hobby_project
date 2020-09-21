#include <server.h>
#include <string>

int main(int argc, char* argv[])
{
	std::string ipAddress = Server::getIpAddress();
	int port = 54000;

	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0) {
		std::cerr << "Can't start Winsock, Err #" << wsResult << std::endl;
		return 0;
	}
	
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}

	// Fill in a hint structure
	sockaddr_in hints;
	hints.sin_family = AF_INET;
	hints.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hints.sin_addr);

	// Connect to server
	int connection_result = connect(clientSocket, (sockaddr*)&hints, sizeof(hints));

	if (connection_result == SOCKET_ERROR) {
		std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}

	char buf[4096];
	std::string userInput;

	do {
		// receive Mat and display on screen

	} while(userInput.size() > 0)

}

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
	char quitProgram;
	char buffer[1024];
	cv::Mat frame;
	int height = 6000;
	int width = 6000;
	int bytes;
	cv::Mat  img = cv::Mat::zeros(height, width, CV_8UC3);
	int  imgSize = img.total() * img.elemSize();
	uchar sockData[1024];
	while (true) {
		bytes = 0;
		for (int i = 0; i < imgSize; i += bytes) {
			if ((bytes = recv(clientSocket, reinterpret_cast<char*>(sockData) + i, imgSize - i, 0)) == -1) {
				std::cerr << "recv failed" << std::endl;
				break;
			}
		}

		// Assign pixel value to img
		int ptr = 0;
		for (int i = 0; i < img.rows; i++) {
			for (int j = 0; j < img.cols; j++) {
				img.at<cv::Vec3b>(i, j) = cv::Vec3b(sockData[ptr + 0], sockData[ptr + 1], sockData[ptr + 2]);
				ptr = ptr + 3;
			}
		}


		quitProgram = cv::waitKey(100);

		if (quitProgram == 'q') {
			closesocket(clientSocket);
			WSACleanup();
			break;
		}
	}


}

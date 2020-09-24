#include <server.h>
#include <string>
// https://answers.opencv.org/question/197414/sendreceive-vector-mat-over-socket-cc/
// https://stackoverflow.com/questions/41637438/opencv-imencode-buffer-exception
int main(int argc, char* argv[])
{
	std::string ipAddress = "192.some.address.here";
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
	char quitProgram = 'n';
	cv::Mat frame;
	std::vector <char> buffer;
	size_t buffer_size = 0;
	recv(clientSocket, (char *)(&buffer_size), sizeof(int), 0);
	buffer.resize(buffer_size);
	std::cout << "buffer.size = " << buffer.size() << std::endl;
	int len = 0;
	size_t remain = buffer_size;
	size_t offset = 0;
	//while ((remain > 0) && ((len = recv(clientSocket, reinterpret_cast<char*>(buffer.data()) + offset, remain, 0)) > 0))
	while ((remain > 0) && ((len = recv(clientSocket, buffer.data() + offset, remain, 0)) > 0))
	{
		remain -= len;
		offset += len;
	}
	if (len <= 0)
	{
		std::cout << "Error processing receipt of packet" << std::endl;
		// handle fatal error
	}

	std::vector <char> buffer_uchar;
	//buffer_uchar.resize(buffer_size);
	std::copy(buffer.begin(), buffer.end(), std::back_inserter(buffer_uchar));
	frame = cv::imdecode(cv::Mat(buffer_uchar), 1); // "If the buffer is too short or contains invalid data, the empty matrix will be returned."
 	cv::imshow("received frame", frame);      // See http://opencv.jp/opencv-2svn_org/cpp/reading_and_writing_images_and_video.html
	quitProgram = cv::waitKey(100);

	if (quitProgram == 'q') {
		closesocket(clientSocket);
		WSACleanup();
	}
}

		/*
		while (true) {
			recv(clientSocket, reinterpret_cast<char*>(&buffer_size), sizeof(int), 0);
			buffer.resize(buffer_size);
			std::cout << "buffer.size = " << buffer.size() << std::endl;




		}
		'/

		/*
		int height = 6000;
		int width = 6000;
		int bytes;
		cv::Mat  img = cv::Mat::zeros(height, width, CV_8UC3);
		int  imgSize = img.total() * img.elemSize();
		uchar sockData[1024] = {};
		while (true) {
		bytes = 0;
		for (int i = 0; i < imgSize; i += bytes) {
		if ((bytes = recv(clientSocket, reinterpret_cast<char*>(sockData) + i, imgSize - i, 0)) == -1) {
		std::cerr << "recv failed" << std::endl;
		break;
		}
		}

		// Assign pixel value to img
		if (sockData[0] != '\0') {
		int ptr = 0;
		for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
		img.at<cv::Vec3b>(i, j) = cv::Vec3b(sockData[ptr + 0], sockData[ptr + 1], sockData[ptr + 2]);
		ptr = ptr + 3;
		}
		}

		cv::imshow("DisplayedImg", img);
		quitProgram = cv::waitKey(100);
		}
		if (quitProgram == 'q') {
		closesocket(clientSocket);
		WSACleanup();
		break;
		}
		}
		*/


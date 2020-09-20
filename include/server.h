#pragma once
#ifndef server__H__
#define server__H__
#include <string>
#include <WS2tcpip.h>

// #pragma comment(lib, "ws2_32.lib") "https://stackoverflow.com/questions/3484434/what-does-pragma-comment-mean"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/trace.hpp>


class Server {



public:  // Prefix "FD" indicates FileDescriptor because all sockets in Unix are file-descriptors
	   
	typedef void (*MessageReceivedCallback)(Server serverListener, int socketID, std::string msg);

	Server(const std::string& ipAddress, int port, MessageReceivedCallback callBack);


	~ Server();
	// Function-Pointer/Callback to Data-Received:

	// Initialise winsock
	bool init();

	// The main processing loop
	void Run();

	// Share frame with client
	void shareFrame(const int& clientSocket, const cv::Mat sharedFrame); // copied val of shared frame?



	// Receive Loop
	//	Send back message

	// Cleanup




private:

	std::string m_ipAddress;
	int m_port;
	MessageReceivedCallback m_messageReceived;

	// Private Methods

	// Create a socket
	SOCKET createSocket();

	// Wait for a connection
	SOCKET waitForConnection();










};
#endif 
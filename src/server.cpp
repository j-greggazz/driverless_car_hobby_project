#include <server.h>
//#define MAX_BUFFER_SIZE (1024)

using namespace cv;


//Server::Server(const std::string& ipAddress, const int& port, MessageReceivedCallback callBack) : m_ipAddress(ipAddress), m_port(port), m_messageReceived(callBack) {}

Server::Server(const std::string& ipAddress, const int& port) : m_ipAddress(ipAddress), m_port(port){}

Server::~Server()
{
}

std::string Server::getIpAddress()
{
	WSADATA wsa_Data;
	int wsa_ReturnCode = WSAStartup(0x101, &wsa_Data);

	// Get the local hostname
	char szHostName[255];
	gethostname(szHostName, 255);
	struct hostent* host_entry;
	host_entry = gethostbyname(szHostName);
	char* szLocalIP;
	szLocalIP = inet_ntoa(*(struct in_addr*)*host_entry->h_addr_list);
	WSACleanup();

	return szLocalIP;
}

bool Server::init()
{
	// Version number of winsock:
	WSAData data;
	WORD ver = MAKEWORD(2, 2);

	int wsInit = WSAStartup(ver, &data);     // The WSAStartup function is called to initiate use of WS2_32.dll

	//if wsInit != 0
	return wsInit == 0;						 // returns true if it worked
}

void Server::run(cv::VideoCapture& vCap, std::atomic<bool>& stopThreads)
{
	// Check for new clients every 50 frames;

	int counter = 0;
	while (true) {
	
		//if ((counter % 50 == 0) | (m_clients.size() == 0)) { // Need to add "select" function if multiple clients are to be added
		if (m_clients.size() == 0) {
			// Create a listening socket:
			SOCKET listener = createSocket();
			if (listener == INVALID_SOCKET) {
				break;
			}
			SOCKET client = waitForConnection(listener);
			if (client != INVALID_SOCKET) {
				closesocket(listener);
				WSACleanup();
				if (std::count(m_clients.begin(), m_clients.end(), client))
					std::cout << "Client already exists" << std::endl;
				else {
					m_clients.push_back(client);
				}
			}
		}
		//while (vCap.isOpened()) {
		if (!m_frame.empty()) {
			size_t frameSize = m_frame.total() * m_frame.elemSize();
			m_sentFrame = (m_frame.reshape(0, 1));
			char* sentData = reinterpret_cast<char*>(m_sentFrame.data);
			for (size_t i = 0; i < m_clients.size(); i++) {
				send(m_clients[i], sentData, frameSize, 0);
			}
		}

		if (stopThreads) {
			for (size_t i = 0; i < m_clients.size(); i++) {
				closesocket(m_clients[i]);
			}
			WSACleanup();
			//closesocket(m_clients[i]);
			break;
			return;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		counter++;
	}
	
}

void Server::shareFrame(const int& clientSocket, cv::Mat sharedFrame)
{
	size_t frameSize = sharedFrame.total() * sharedFrame.elemSize();
	sharedFrame = (sharedFrame.reshape(0, 1));
	//uchar* data = sharedFrame.data;
	send(clientSocket, reinterpret_cast<char*>(sharedFrame.data), frameSize, 0);  //https://stackoverflow.com/questions/49165877/incompatibility-between-char-and-unsigned-char
}

std::thread Server::serverThread(cv::VideoCapture& vCap, std::atomic<bool>& stopThreads) {

	return std::thread(&Server::run, this, std::ref(vCap), std::ref(stopThreads));
}


void Server::setFrame(const cv::Mat& frame)
{
	m_sentFrame = frame;
}

void Server::cleanup()
{
	WSACleanup();         // The WSACleanup function terminates use of the Winsock 2 DLL (Ws2_32.dll).
}

SOCKET Server::createSocket()
{
	SOCKET listener = socket(AF_INET, SOCK_STREAM, 0);   			// SOCK_STREAM for TCP
																	// AF_INET is an address family that is used to designate the type of addresses 
																	// that your socket can communicate with (in this case, Internet Protocol v4 addresses)
	if (listener != INVALID_SOCKET) {
		sockaddr_in hints;                               			// The hints parameter specifies the preferred socket type, or protocol.
		hints.sin_family = AF_INET;									// The field "sin_family" contains a code for the address family, (always set to the symbolic constant AF_INET)															   
		hints.sin_port = htons(m_port);					 			// The htons() function makes sure that numbers are stored in memory in network byte order, 
																	// which is with the most significant byte first. It will therefore swap the bytes making up
																	// the number so that in memory the bytes will be stored in the order

		inet_pton(AF_INET, m_ipAddress.c_str(), &hints.sin_addr);   // The inet_pton() function converts an Internet address in its standard text format into its numeric binary form

		int bind_ok = bind(listener, (sockaddr*)&hints, sizeof(hints));

		if (bind_ok != SOCKET_ERROR) {
			//int listenOk = listen(listener, SOMAXCONN);             // Socket Outstanding MAX CONNections
			if (listen(listener, SOMAXCONN) == SOCKET_ERROR)
				return -1;
		}
		else {
			return -1;
		}
	}
	return listener;
}

SOCKET Server::waitForConnection(const SOCKET& listener)
{
	SOCKET client = accept(listener, NULL, NULL); // waits for connection
	std::cout << "Established connection from " << client << std::endl;
	return client;
}


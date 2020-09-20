#include <server.h>


using namespace cv;
using namespace std;

Server::Server(const std::string& ipAddress, int port, MessageReceivedCallback callBack): m_ipAddress(ipAddress), m_port(port), m_messageReceived(callBack){}

Server::~Server()
{
}

bool Server::init()
{
	return false;
}

void Server::Run()
{
}

void Server::shareFrame(const int& clientSocket, const cv::Mat sharedFrame)
{
}

SOCKET Server::createSocket()
{
	return SOCKET();
}

SOCKET Server::waitForConnection()
{
	return SOCKET();
}


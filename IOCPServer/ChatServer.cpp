#include "ChatServer.h"

void ChatServer::OnConnect(unsigned int clientIndx)
{
	std::cout << "OnConnect : " << clientIndx << "\r\n";
}
void ChatServer::OnClose(unsigned int clientIndx)
{
	std::cout << "OnClose : " << clientIndx << "\r\n";
}
void ChatServer::OnReceive(unsigned int clientIndx, char* pData, DWORD dwDataSize)
{
	std::cout << "OnReceive : " << clientIndx << "\r\n";
}
void ChatServer::Run(const unsigned int MaxClientCount)
{
	packetManager.Init(MaxClientCount);
	packetManager.Run();

	StartServer(MaxClientCount);
}
void ChatServer::End()
{
	DestroyThread();
}




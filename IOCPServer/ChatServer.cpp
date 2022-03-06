#include "ChatServer.h"

void ChatServer::OnConnect(unsigned int clientIndx)
{
	std::cout << "OnConnect : " << clientIndx << "\r\n";
	mPacketManager.PushSystemInfo(clientIndx,SYSTEM_INFO::CONNECT);

}
void ChatServer::OnClose(unsigned int clientIndx)
{
	std::cout << "OnClose : " << clientIndx << "\r\n";
	mPacketManager.PushSystemInfo(clientIndx, SYSTEM_INFO::DISCONNECT);
}
void ChatServer::OnReceive(unsigned int clientIndx, char* pData, DWORD dwDataSize)
{
	std::cout << "OnReceive : " << clientIndx << "\r\n";
	mPacketManager.PushReceiveData(clientIndx, pData, dwDataSize);
}
void ChatServer::Run(const unsigned int MaxClientCount)
{
	mPacketManager.Init(MaxClientCount);
	mPacketManager.Run();

	StartServer(MaxClientCount);
}
void ChatServer::End()
{
	DestroyThread();
}




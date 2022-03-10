#include "ChatServer.h"
#include "PacketDefine.h"

void ChatServer::OnConnect(unsigned int clientIndx)
{
	std::cout << "OnConnect : " << clientIndx << "\r\n";
	mPacketManager.PushSystemInfo(clientIndx, PACKET_ID::PACKET_ID_CONNECT);

}
void ChatServer::OnClose(unsigned int clientIndx)
{
	std::cout << "OnClose : " << clientIndx << "\r\n";
	mPacketManager.PushSystemInfo(clientIndx, PACKET_ID::PACKET_ID_DISCONNECT);
}
void ChatServer::OnReceive(unsigned int clientIndx, char* pData, DWORD dwDataSize)
{
	std::cout << "OnReceive : " << clientIndx << "\r\n";
	mPacketManager.PushReceiveData(clientIndx, pData, dwDataSize);
}
void ChatServer::Run(const unsigned int MaxClientCount)
{
	auto sendPacketFunc = [&](UINT32 clientIndex, char* pSendPacket, UINT16 packetSize)
	{
		SendMsg(clientIndex, packetSize, pSendPacket);
	};
	mPacketManager.SendPacketFunc = sendPacketFunc;
	mPacketManager.Init(MaxClientCount);
	mPacketManager.Run();

	StartServer(MaxClientCount);
}
void ChatServer::End()
{
	mPacketManager.End();
	DestroyThread();
}




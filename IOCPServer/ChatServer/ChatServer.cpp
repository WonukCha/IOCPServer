#include "ChatServer.h"
#include "PacketDefine.h"

void ChatServer::OnConnect(unsigned int clientIndx)
{
	//std::cout << "OnConnect : " << clientIndx << "\r\n";
	mPacketManager.PushSystemInfo(clientIndx, PACKET_ID::SYSYEM_CONNECT);

}
void ChatServer::OnClose(unsigned int clientIndx)
{
	//std::cout << "OnClose : " << clientIndx << "\r\n";
	mPacketManager.PushSystemInfo(clientIndx, PACKET_ID::SYSYEM_DISCONNECT);
}
void ChatServer::OnReceive(unsigned int clientIndx, char* pData, DWORD dwDataSize)
{
	//std::cout << "OnReceive : " << clientIndx << " Size :"<< dwDataSize << "\r\n";
	mPacketManager.PushReceiveData(clientIndx, pData, dwDataSize);
}
void ChatServer::Run(const unsigned int MaxClientCount)
{
	auto sendPacketFunc = [&](UINT32 clientIndex, char* pSendPacket, UINT32 packetSize)
	{
		if (SendMsg(clientIndex, packetSize, pSendPacket) == false)
		{
			//std::cout << "sendPacketFunc Faile\r\n";
		}
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




#pragma once
#include "IOCPServer.h"
#include "PacketManager.h"

class ChatServer : public IOCPServer
{
public:
	ChatServer() = default;
	virtual ~ChatServer() = default;

	void OnConnect(unsigned int clientIndx) override;
	void OnClose(unsigned int clientIndx) override;
	void OnReceive(unsigned int clientIndx, char* pData, DWORD dwDataSize) override;

	void Run(const unsigned int MaxClientCount);
	void End();

private:
	PacketManager mPacketManager;
};

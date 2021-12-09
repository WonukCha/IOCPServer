#pragma once
#include "IOCPServer.h"

class EchoServer : public IOCPServer
{
	virtual void OnConnect(unsigned int clientIndx) override;
	virtual void OnClose(unsigned int clientIndx) override;
	virtual void OnReceive(unsigned int clientIndx, RingbufferLock* pRingbuf) override;
};


#pragma once
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "mswsock.lib")

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#include <iostream>

#include <thread>
#include <vector>

class IOCPServer
{
public:
	IOCPServer(void);
	virtual ~IOCPServer(void);

//protected:
	bool Init(unsigned int MaxThreadCount);
	bool BindListen(unsigned short port);

private:
	unsigned int mMaxIocpWorkerThreadCount = 0;

	SOCKET	mListenSocket = INVALID_SOCKET;

	HANDLE	mIOCPHandle = INVALID_HANDLE_VALUE;
};



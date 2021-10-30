#pragma once
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "mswsock.lib")

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#include <iostream>

#include <thread>
#include <vector>
#include <thread>

#define MAX_WORK_THREAD_COUNT 5

class IOCPServer
{
public:
	IOCPServer(void);
	virtual ~IOCPServer(void);

//protected:
	bool Init(unsigned int MaxThreadCount);
	bool BindListen(unsigned short Port);
	bool StartServer(const unsigned int MaxClientCount);
	void DestroyThread();

private:
	unsigned int mMaxIocpWorkerThreadCount = 0;
	SOCKET	mListenSocket = INVALID_SOCKET;
	HANDLE	mIOCPHandle = INVALID_HANDLE_VALUE;
	std::vector<std::thread> mWorkThread;
	std::thread mAcceptThread;
	bool mIsWorkerRun = true;
	bool mIsAcceptRun = true;

	void CreateClient(const unsigned int MaxClientCount);
	void WorkThread();
	void AcceptThread();
};



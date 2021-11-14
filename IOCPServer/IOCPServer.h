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

#include "ClientInfo.h"
#include "define.h"


constexpr UINT MAX_WORK_THREAD_COUNT = 5;

class IOCPServer
{
public:
	IOCPServer(void);
	virtual ~IOCPServer(void);

	//virtual void OnConnect(unsigned int clientIndx) PURE;
	//virtual void OnClose(unsigned int clientIndx) PURE;
	//virtual void OnReceive(unsigned int clientIndx) PURE;

	void OnConnect(unsigned int clientIndx);
	void OnClose(unsigned int clientIndx);
	void OnReceive(unsigned int clientIndx);


//protected:
	bool Init(unsigned int maxThreadCount);
	bool BindListen(unsigned short port);
	bool StartServer(const unsigned int maxClientCount);
	void DestroyThread();

private:
	void CreateClient(const unsigned int maxClientCount);
	void WorkThread();
	void AcceptThread();
	ClientInfo* GetClientInfo(unsigned int index);
	void CloseSocket(ClientInfo* clientInfo, bool bLingerOn = false);

	unsigned int mMaxIocpWorkerThreadCount = 0;
	std::vector<ClientInfo*> mClientInfos;
	SOCKET	mListenSocket = INVALID_SOCKET;
	HANDLE	mIOCPHandle = INVALID_HANDLE_VALUE;
	//vector? arr? »Ï..
	std::thread mWorkThread[MAX_WORK_THREAD_COUNT];
	std::thread mAcceptThread;
	bool mIsWorkerRun = true;
	bool mIsAcceptRun = true;
	//atomic¿∏∑Œ ∫Ø»Ø
	unsigned int mClientCnt = 0;

};



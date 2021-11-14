#include "IOCPServer.h"
#include "ClientInfo.h"

IOCPServer::IOCPServer(void)
{
}

IOCPServer::~IOCPServer(void)
{
	WSACleanup();

}

void IOCPServer::OnConnect(unsigned int clientIndx) {};
void IOCPServer::OnClose(unsigned int clientIndx) {};
void IOCPServer::OnReceive(unsigned int clientIndx) {};

bool IOCPServer::Init(unsigned int MaxThreadCount)
{
	bool bFuncResult = false;
	WSADATA wsaData;
	ZeroMemory(&wsaData,sizeof(wsaData));

	do 
	{
		int nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (0 != nResult)
		{
			std::cout << "[Error] WSAStartup : " << WSAGetLastError() <<" Function :" << __FUNCTION__ << " Line :" << __LINE__ << "/r/n";
			break;
		}
		mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

		if (mListenSocket == INVALID_SOCKET)
		{
			std::cout << "[Error] Create ListenSocket : " << WSAGetLastError() << " Function :" << __FUNCTION__ << " Line :" << __LINE__ << "/r/n";
			break;
		}
		mMaxIocpWorkerThreadCount = MaxThreadCount;

		bFuncResult = true;
	} while (false);


	return bFuncResult;
}

bool IOCPServer::BindListen(unsigned short port)
{
	bool bFuncResult = false;
	SOCKADDR_IN		stServerInfo;
	stServerInfo.sin_addr.s_addr = htonl(INADDR_ANY);
	stServerInfo.sin_port = port;
	stServerInfo.sin_family = AF_INET;

	do
	{
		int nResult = bind(mListenSocket, (SOCKADDR*)&stServerInfo, sizeof(SOCKADDR_IN));
		if (0 != nResult)
		{
			std::cout << "[Error] bind : " << WSAGetLastError() << " Function :" << __FUNCTION__ << " Line :" << __LINE__ << "/r/n";
			break;
		}
		__LINE__;

		nResult = listen(mListenSocket, 5);
		if (0 != nResult)
		{
			std::cout << "[Error] Listen : " << WSAGetLastError() << " Function :" << __FUNCTION__ << " Line :" << __LINE__ << "/r/n";
			break;
		}

		mIOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, mMaxIocpWorkerThreadCount);
		if (INVALID_HANDLE_VALUE == mIOCPHandle)
		{
			std::cout << "[Error] CreateIoCompletionPort : " << GetLastError() << " Function :" << __FUNCTION__ << " Line :" << __LINE__ << "/r/n";
			break;
		}
		HANDLE hResult = CreateIoCompletionPort((HANDLE)mListenSocket,mIOCPHandle,(ULONG_PTR)0,(DWORD)0);
		if (NULL == hResult)
		{
			std::cout << "[Error] ListenSocket IOCP Bind Fail : " << WSAGetLastError() << " Function :" << __FUNCTION__ << " Line :" << __LINE__ << "/r/n";
			break;
		}
		
		std::cout << "Start Server\r\n";
		bFuncResult = true;
	} while (false);


	return bFuncResult;
}

bool IOCPServer::StartServer(const unsigned int MaxClientCount)
{
	bool bResult = false;

	CreateClient(MaxClientCount);

	for (unsigned int i = 0; i < MAX_WORK_THREAD_COUNT; i++)
	{
		mWorkThread[i] = std::thread([this]() {this->WorkThread(); });
	}

	mAcceptThread = std::thread([this]() {this->AcceptThread(); });

	bResult = true;

	return bResult;
}

void IOCPServer::DestroyThread()
{
	mIsWorkerRun = false;
	CloseHandle(mIOCPHandle);

	for (auto& thread : mWorkThread)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}

	mIsAcceptRun = false;
	closesocket(mListenSocket);

	if (mAcceptThread.joinable())
	{
		mAcceptThread.join();
	}
}

void IOCPServer::CreateClient(const unsigned int MaxClientCount)
{
	for (unsigned int i = 0; i < MaxClientCount; i++)
	{
		auto client = new ClientInfo();
		client->Init(i, mIOCPHandle);
		mClientInfos.push_back(client);
	}
}

void IOCPServer::WorkThread()
{
	ClientInfo* pClientInfo = nullptr;
	BOOL bSuccess = TRUE;
	DWORD dwIoSize = 0;
	LPOVERLAPPED lpOverlapped = NULL;

	while (true)
	{
		if (mIsWorkerRun == false)
			break;

		bSuccess = GetQueuedCompletionStatus(
			mIOCPHandle,
			&dwIoSize,
			(ULONG_PTR*)pClientInfo,
			&lpOverlapped,
			INFINITE);

		if (TRUE == bSuccess && 0 == dwIoSize && NULL == lpOverlapped)
		{
			mIsWorkerRun = false;
			continue;
		}

		if (NULL == lpOverlapped)
		{
			continue;
		}

		stOverlappedEx* pOverlappedEx = (stOverlappedEx*)lpOverlapped;

		if (FALSE == bSuccess || (0 == dwIoSize && IOOperation::ACCEPT != pOverlappedEx->m_eOperation))
		{
			CloseSocket(pClientInfo);
			continue;
		}

		if (IOOperation::ACCEPT == pOverlappedEx->m_eOperation)
		{
			pClientInfo = GetClientInfo(pOverlappedEx->SessionIndex);
			if (pClientInfo->AcceptCompletion())
			{
				//++mClientCnt;

				OnConnect(pClientInfo->GetClientIndex());
			}
			else
			{
				CloseSocket(pClientInfo); 
			}
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}
void IOCPServer::AcceptThread()
{
	while (true)
	{
		if (mIsAcceptRun == false)
			break;
		auto curTimeSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

		for (auto client : mClientInfos)
		{
			if (client->IsConnect())
			{
				continue;
			}

			if ((UINT64)curTimeSec < client->GetLatestClosedTimeSec())
			{
				continue;
			}

			auto diff = curTimeSec - client->GetLatestClosedTimeSec();
			if (diff <= RE_USE_SESSION_WAIT_TIMESEC)
			{
				continue;
			}

			client->PostAccept(mListenSocket, curTimeSec);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(32));
	}
}

ClientInfo* IOCPServer::GetClientInfo(unsigned int index)
{
	ClientInfo* pClinetInfo = nullptr;
	do
	{
		if (index > mClientInfos.size() -1)
			break;
		pClinetInfo = mClientInfos[index];

	} while (false);

	return pClinetInfo;
}
void IOCPServer::CloseSocket(ClientInfo* clientInfo, bool bLingerOn)
{
	do
	{
		if (clientInfo == nullptr)
			break;

		if (clientInfo->IsConnect() == false)
			break;

		unsigned int index = clientInfo->GetClientIndex();
		
		clientInfo->Close();

		//--mClientCnt;

		OnClose(index);

	} while (false);
}
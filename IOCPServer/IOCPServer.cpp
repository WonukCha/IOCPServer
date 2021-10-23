#include "IOCPServer.h"


IOCPServer::IOCPServer(void)
{

}

IOCPServer::~IOCPServer(void)
{
	WSACleanup();
}

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

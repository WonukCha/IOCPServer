#include "ClientInfo.h"

ClientInfo::ClientInfo()
{
	ZeroMemory(&mRecvOverlappedEx, sizeof(mRecvOverlappedEx));
	ZeroMemory(&mAcceptContext, sizeof(mAcceptContext));

}
ClientInfo::~ClientInfo()
{

}

void ClientInfo::Init(const unsigned int idex, const HANDLE iocpHandle)
{
	mClinetIndex = idex;
	mIocpHandle = iocpHandle;
}

void ClientInfo::SetSocket(SOCKET socket)
{
	mSocket = socket;
}
SOCKET ClientInfo::GetSocket()
{
	return mSocket;
}

bool ClientInfo::IsConnect()
{
	return mIsConnect;
}

unsigned int ClientInfo::GetClientIndex()
{
	return mClinetIndex;
}
void ClientInfo::Close(bool bLingerOn)
{
	struct linger stLinger = { 0, 0 };

	if (true == bLingerOn)
	{
		stLinger.l_onoff = 1;
	}

	shutdown(mSocket, SD_BOTH);

	setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

	mIsConnect = false;
	mLatestClosedTimeSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

	closesocket(mSocket);
	mSocket = INVALID_SOCKET;
}
bool ClientInfo::AcceptCompletion()
{
	bool bResult = false;

	do
	{
		if (OnConnect(mIocpHandle, mSocket) == false)
			break;

		bResult = true;
	} while (false);
	
	return bResult;
}
bool ClientInfo::OnConnect(HANDLE iocpHandle_, SOCKET socket_)
{
	bool bResult = false;

	do 
	{
		mSocket = socket_;
		mIsConnect = true;

		if (BindIOCompletionPort(iocpHandle_) == false)
			break;

		BindRecv();
		bResult = true;
	} while (false);

	return(bResult);
}
bool ClientInfo::BindIOCompletionPort(HANDLE iocpHandle_)
{
	bool bResult = false;
	do
	{
		HANDLE hIOCP = CreateIoCompletionPort((HANDLE)GetSocket()
			, iocpHandle_
			, (ULONG_PTR)(this), 0);

		if (hIOCP == INVALID_HANDLE_VALUE)
			break;

		bResult = true;
	} while (false);
	
	return bResult;
}
bool ClientInfo::BindRecv()
{
	bool bResult = false;
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	do
	{
		mRecvOverlappedEx.m_wsaBuf.len = MAX_SOCK_RECVBUF;
		mRecvOverlappedEx.m_wsaBuf.buf = mRecvBuf;
		mRecvOverlappedEx.m_eOperation = IOOperation::RECV;

		int nRet = WSARecv(mSocket,
			&(mRecvOverlappedEx.m_wsaBuf),
			1,
			&dwRecvNumBytes,
			&dwFlag,
			(LPWSAOVERLAPPED) & (mRecvOverlappedEx),
			NULL);

		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
			break;
			
		bResult = true;
	} while (false);

	return bResult;
}

UINT64 ClientInfo::GetLatestClosedTimeSec()
{
	return mLatestClosedTimeSec;
}
bool ClientInfo::PostAccept(SOCKET listenSock_, const UINT64 curTimeSec_)
{
	bool bResult = false;

	do
	{
		mLatestClosedTimeSec = UINT32_MAX;

		mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP,
			NULL, 0, WSA_FLAG_OVERLAPPED);

		if (INVALID_SOCKET == mSocket)
			break;

		ZeroMemory(&mAcceptContext, sizeof(stOverlappedEx));

		DWORD bytes = 0;
		DWORD flags = 0;
		mAcceptContext.m_wsaBuf.len = 0;
		mAcceptContext.m_wsaBuf.buf = nullptr;
		mAcceptContext.m_eOperation = IOOperation::ACCEPT;
		mAcceptContext.SessionIndex = mClinetIndex;

		if (FALSE == AcceptEx(listenSock_, mSocket, mAcceptBuf, 0,
			sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, (LPWSAOVERLAPPED) & (mAcceptContext)))
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
				break;
		}

		bResult = true;
	} while (false);

	return bResult;
}
char* ClientInfo::RecvBuffer()
{
	return mRecvBuf;
}

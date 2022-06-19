#include "ClientInfo.h"
#include <iostream>

ClientInfo::ClientInfo()
{
	ZeroMemory(&mRecvOverlappedEx, sizeof(mRecvOverlappedEx));
	ZeroMemory(&mAcceptContext, sizeof(mAcceptContext));
	mIsSending.store(false);
}
ClientInfo::~ClientInfo()
{

}

//ClientInfo::ClientInfo(ClientInfo&& other) noexcept // 이동 생성자 선언
//{
//
//}
//
//ClientInfo& ClientInfo::operator=(ClientInfo&& other) noexcept // 이동 복사 연산자 선언
//{
//
//}

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

char* ClientInfo::GetRecvBuf()
{
	return mRecvBuf;
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

	mIsSending.store(false);
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
	//std::cout << "PostAccept. client Index: "<< GetClientIndex() << "\r\n";

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
bool ClientInfo::SendMsg(const unsigned __int64 uiMsgSize, void* pMsg)
{
	bool bResult = false;
	do
	{
		if (mSendRingbuffer.PutData(pMsg, uiMsgSize) == false)
		{
			std::cout << "[Error] mSendRingbuffer.PutData \r\n";
			break;
		}

		if (mIsSending.load())
		{
			break;
		}
		else
		{
			mIsSending.store(true);
		}

		if (SendIO() == false)
			break;

		bResult = true;
	} while (false);
	return bResult;
}
void ClientInfo::SendCompleted(const unsigned int uiMsgSize)
{
	//std::cout << "Client : " << GetClientIndex() << ", Send Size : " << uiMsgSize << "\r\n";

	mSendRingbuffer.DeleteData(uiMsgSize);
	if (mSendRingbuffer.GetSize() > 0)
	{
		SendIO();
	}
	else
	{
		mIsSending.store(false);
	}
}
bool ClientInfo::SendIO()
{
	bool bResult = false;
	do
	{
		size_t szSendSize = 0;
		if (MAX_SOCK_SENDBUF < mSendRingbuffer.GetSize())
			szSendSize = MAX_SOCK_SENDBUF;
		else
			szSendSize = mSendRingbuffer.GetSize();

		if (ULONG_MAX < szSendSize)
			szSendSize = ULONG_MAX;

		if (MAX_SOCK_SENDBUF < szSendSize)
			szSendSize = MAX_SOCK_SENDBUF;

		if (mSendRingbuffer.GetData(mSendBuf, szSendSize) == false)
		{
			std::cout << "[Error] SendIO - SendRingBuf \r\n";
			break;
		}

		mSendOverlappedEx.m_wsaBuf.buf = mSendBuf;
		mSendOverlappedEx.m_wsaBuf.len = static_cast<ULONG>(szSendSize);
		mSendOverlappedEx.m_eOperation = IOOperation::SEND;


		DWORD dwSendNumBytes = 0;
		int nRet = WSASend(mSocket,
			&(mSendOverlappedEx.m_wsaBuf),
			1,
			&dwSendNumBytes,
			0,
			(LPWSAOVERLAPPED)&mSendOverlappedEx,
			NULL);

		//socket_error이면 client socket이 끊어진걸로 처리한다.
		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			std::cout << "[Error] WSASend()함수 실패 : " << WSAGetLastError() << "\r\n";
			break;
		}

		bResult = true;
	} while (false);

	return bResult;
}

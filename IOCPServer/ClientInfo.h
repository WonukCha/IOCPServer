#pragma once
#include "Define.h"
#include "RingBufferLock.hpp"

class ClientInfo
{
public:
	ClientInfo();
	~ClientInfo();

	void Init(const unsigned int idex, const HANDLE iocpHandle);

	void SetSocket(SOCKET socket);
	SOCKET GetSocket();

	bool IsConnect();
	unsigned int GetClientIndex();

	void Close(bool bLingerOn = false);
	bool AcceptCompletion();
	bool OnConnect(HANDLE iocpHandle_, SOCKET socket_);
	bool BindIOCompletionPort(HANDLE iocpHandle_);
	bool BindRecv();
	UINT64 GetLatestClosedTimeSec();
	bool PostAccept(SOCKET listenSock_, const UINT64 curTimeSec_);

	char* GetRecvBuf();

	bool SendMsg(const unsigned __int64 uiMsgSize, void* pMsg);
	void SendCompleted(const unsigned int uiMsgSize);
	bool SendIO();
protected:

private:

	unsigned int mClinetIndex = 0;
	HANDLE mIocpHandle = INVALID_HANDLE_VALUE;
	SOCKET mSocket = INVALID_SOCKET;
	bool mIsConnect = false;
	UINT64 mLatestClosedTimeSec = 0;
	
	stOverlappedEx	mSendOverlappedEx;
	char mSendBuf[MAX_SOCK_SENDBUF] = { 0, };
	RingbufferLock mSendRingbuffer;
	std::mutex mSendMutex;

	stOverlappedEx	mRecvOverlappedEx;
	char mRecvBuf[MAX_SOCK_RECVBUF] = { 0, };
	
	stOverlappedEx	mAcceptContext;
	char mAcceptBuf[64] = { 0, };
};


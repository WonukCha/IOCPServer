#pragma once

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>

constexpr UINT32 MAX_SOCK_RECVBUF = 4096;	// 소켓 버퍼의 크기
constexpr UINT32 MAX_SOCK_SENDBUF = 4096;	// 소켓 버퍼의 크기
constexpr UINT64 RE_USE_SESSION_WAIT_TIMESEC = 3;

enum class IOOperation
{
	ACCEPT,
	RECV,
	SEND
};

//WSAOVERLAPPED구조체를 확장 시켜서 필요한 정보를 더 넣었다.
struct stOverlappedEx
{
	WSAOVERLAPPED m_wsaOverlapped = { 0 , };		//Overlapped I/O구조체
	WSABUF		m_wsaBuf = { 0 ,};				//Overlapped I/O작업 버퍼
	IOOperation m_eOperation = IOOperation::ACCEPT;			//작업 동작 종류
	UINT32 SessionIndex = 0;
};

#pragma once

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>

constexpr UINT32 MAX_SOCK_RECVBUF = 4096;	// ���� ������ ũ��
constexpr UINT32 MAX_SOCK_SENDBUF = 4096;	// ���� ������ ũ��
constexpr UINT64 RE_USE_SESSION_WAIT_TIMESEC = 3;

enum class IOOperation
{
	ACCEPT,
	RECV,
	SEND
};

//WSAOVERLAPPED����ü�� Ȯ�� ���Ѽ� �ʿ��� ������ �� �־���.
struct stOverlappedEx
{
	WSAOVERLAPPED m_wsaOverlapped = { 0 , };		//Overlapped I/O����ü
	WSABUF		m_wsaBuf = { 0 ,};				//Overlapped I/O�۾� ����
	IOOperation m_eOperation = IOOperation::ACCEPT;			//�۾� ���� ����
	UINT32 SessionIndex = 0;
};

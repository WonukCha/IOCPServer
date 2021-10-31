#pragma once
#include "Define.h"

class ClientInfo
{
public:
	ClientInfo();
	~ClientInfo();

	void Init(const unsigned int idex, const HANDLE iocpHandle);
protected:

private:
	unsigned int mClinetIndex = 0;
	HANDLE mIocpHandle = INVALID_HANDLE_VALUE;
	SOCKET mSocket = INVALID_SOCKET;
};


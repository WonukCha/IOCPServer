#pragma once
#include <string>

#include "RingBufferLock.hpp"
#include "PacketDefine.h"

class User
{
public:
	void Init();

	bool PushLowData(char* pData, UINT32 dwDataSize);

	PacketInfo GetPacketInfo();
	void SetUserStatus(USER_STATUS_INFO status);
	USER_STATUS_INFO GetUserStatus();

	void SetUserIndex(UINT32 userIndex);
	UINT32 GetUserIndex();

private:
	UINT32 mUserIndex = UINT32_MAX;
	USER_STATUS_INFO mUserStatus = USER_STATUS_INFO::NONE;
	std::string mId;
	RingBufferLock mRecvBuffer;
	char mStageBuffer[USER_BUFFER_SIZE];
};
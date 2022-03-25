#pragma once
#include <string>
#include <iostream>

#include "RingBufferLock.hpp"
#include "PacketDefine.h"
#include "zlib.h"

constexpr UINT16 USER_BUFFER_SIZE = 1204;

class User
{
public:
	void Init();

	bool PushLowData(char* pData, UINT32 dwDataSize);

	PacketInfo* GetPacketInfo();
	void SetUserStatus(USER_STATUS_INFO status);
	USER_STATUS_INFO GetUserStatus();

	void SetUserIndex(UINT32 userIndex);
	UINT32 GetUserIndex();

	void EnterRoom(UINT16 roomNumber);
	void LeaveRoom();
	UINT16 GetCurrentRoom();
private:
	bool CheckPacketHeader(PacketHeader* header);
	 
	UINT32 mUserIndex = UINT32_MAX;
	USER_STATUS_INFO mUserStatus = USER_STATUS_INFO::NONE;
	UINT16 mRoomNumber = UINT16_MAX;;

	std::string mId;

	RingBufferLock mRecvBuffer;
	PacketInfo mStagePacketInfo;
	char mCompressBuffer[USER_BUFFER_SIZE] = { '\0', };
	char mStageBuffer[USER_BUFFER_SIZE] = {'\0',};
};
#pragma once
#include "PacketDefine.h"

enum class RedisTaskID : UINT16
{
	NONE = 0,

	REDIS_REQUEST_LOGIN = 1001,
	REDIS_RESPONSE_LOGIN = 1002,
};

#pragma pack(push, 1)
struct REDIS_HEADER
{
	UINT32 clientNum = 0;
	RedisTaskID RedisTaskId = RedisTaskID::NONE;
	UINT16 dataSize = 0;
	unsigned __int64 tickCount = 0;
	char* pData = nullptr;
};

struct REDIS_REQUEST_LOGIN : public REDIS_HEADER
{
	char ID[ID_SIZE] = { '\0', };
	char PW[PW_SIZE] = { '\0', };
};

struct REDIS_RESPONSE_LOGIN : public REDIS_HEADER
{
	UINT16 Result = (UINT16)false;
};
#pragma pack(pop)
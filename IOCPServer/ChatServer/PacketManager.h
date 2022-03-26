#pragma once
#include <thread>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <functional>
#include <iostream>

#include <basetsd.h>
#include <WTypesbase.h>

#include "PacketDefine.h"
#include "UserManager.h"
#include "RoomManager.h"
#include "RedisManager.h"

#include "zlib.h"

class PacketManager
{
public:
	PacketManager() = default;
	virtual ~PacketManager() = default;

	void Init(const UINT32 maxClientCount);
	void Run();
	void End();

	void PushSystemInfo(UINT32 clientIndx, PACKET_ID systeminfo);
	void PushReceiveData(UINT32 clientIndx, char* pData, UINT32 dataSize);

	std::function<void(UINT32, char*, UINT32)> SendPacketFunc;
private:
	void PacketProcess();

	void ProcessSystemConnect(UINT32 clientIndx, char* pData, UINT32 dataSize);
	void ProcessSystemDisonnect(UINT32 clientIndx, char* pData, UINT32 dataSize);

	void ProcessLogin(UINT32 clientIndx, char* pData, UINT32 dataSize);

	void ProcessAllUserChatMessage(UINT32 clientIndx, char* pData, UINT32 dataSize);
	void ProcessRoomChatMessage(UINT32 clientIndx, char* pData, UINT32 dataSize);

	void ProcessEnterRoom(UINT32 clientIndx, char* pData, UINT32 dataSize);
	void ProcessLeaveRoom(UINT32 clientIndx, char* pData, UINT32 dataSize);

	INT16 CompressPacket(void* pDest,rsize_t* pDestSize, PacketInfo* pPacketInfo, COMPRESS_TYPE compressType);
	INT16 UncompressPacket(void* pDest, rsize_t* pDestSize, PacketInfo* pPacketInfo);

	typedef void(PacketManager::* PROCESS_RECV_PACKET_FUNCTION)(UINT32,char*, UINT32);
	std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION> mProcMap;

	UserManager mUserManager;
	RoomManager mRoomManager;

	std::mutex mSystemInfoQueueLock;
	std::queue<PacketInfo> mSystemInfoQueue;

	std::mutex mUserReceiveEventQueueLock;
	std::queue<UINT32> mUserReceiveEventQueue;

	bool mIsRunProcessThread;
	std::thread mProcessThread;

	Bytef mCompressBuffer[USER_BUFFER_SIZE] = {'\0'};
};


#pragma once
#include<thread>
#include <unordered_map>
#include <deque>

#include <basetsd.h>
#include <WTypesbase.h>

#include "PacketDefine.h"
#include "UserManager.h"

class PacketManager
{
public:
	PacketManager() = default;
	~PacketManager() = default;

	void Init(const unsigned int maxClientCount);
	void Run();
	void End();

	void PushSystemInfo(unsigned int clientIndx, SYSTEM_INFO systeminfo);
	void PushReceiveData(unsigned int clientIndx, char* pData, DWORD dwDataSize);

private:
	void PacketProcess();

	typedef void(PacketManager::* PROCESS_RECV_PACKET_FUNCTION)(UINT32,UINT16,char*);
	std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION> mProcMap;

	UserManager mUserManager;

	bool mIsRunProcessThread;
	std::thread mProcessThread;
};


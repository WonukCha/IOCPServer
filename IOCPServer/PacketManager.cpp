#include "PacketManager.h"

void PacketManager::Init(const unsigned int maxClientCount)
{
	std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION> init;
	mProcMap.swap(init);
}
void PacketManager::Run()
{
	mIsRunProcessThread = true;
	mProcessThread = std::thread([this]() {this->PacketProcess(); });
}
void PacketManager::End()
{
	mIsRunProcessThread = false;
	if (mProcessThread.joinable())
	{
		mProcessThread.join();
	}

}
void PacketManager::PacketProcess()
{
	while (true)
	{
		if (mIsRunProcessThread == false)
			break;


		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}

void PacketManager::PushSystemInfo(unsigned int clientIndx, SYSTEM_INFO systeminfo)
{

}
void PacketManager::PushReceiveData(unsigned int clientIndx, char* pData, DWORD dwDataSize)
{

}
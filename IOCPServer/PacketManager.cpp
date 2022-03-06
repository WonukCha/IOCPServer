#include "PacketManager.h"

void PacketManager::Init(const unsigned int maxClientCount)
{

}
void PacketManager::Run()
{
	mProcessThread = std::thread([this]() {this->PacketProcess(); });
}
void PacketManager::End()
{

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
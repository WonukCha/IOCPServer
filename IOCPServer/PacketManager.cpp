#include "PacketManager.h"

void PacketManager::Init(const UINT32 maxClientCount)
{

	mProcMap[static_cast<int>(PACKET_ID::PACKET_ID_DISCONNECT)] = &PacketManager::ProcessSystemDisonnect;
	mProcMap[static_cast<int>(PACKET_ID::PACKET_ID_CONNECT)]= &PacketManager::ProcessSystemConnect;
	mProcMap[static_cast<int>(PACKET_ID::PACKET_ID_CLIENT_TO_SERVER_CHATTING)]= &PacketManager::ProcessReceiveChat;

	mUserManager.SendPacketFunc = SendPacketFunc;
	mUserManager.Init(maxClientCount);
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

		if (!mSystemInfoQueue.empty())
		{
			PacketInfo info;
			std::queue<PacketInfo> queue;
			mSystemInfoQueueLock.lock();
			mSystemInfoQueue.swap(queue);
			mSystemInfoQueueLock.unlock();

			while (!queue.empty())
			{
				info = queue.front(); queue.pop();
				auto iter = mProcMap.find(info.packetId);
				if (iter != mProcMap.end())
				{
					(this->*(iter->second))(info.clientNum,info.pData,info.dataSize);
				}
			}
		}

		if (!mUserReceiveEventQueue.empty())
		{
			UINT32 userIndex = UINT32_MAX;
			PacketInfo info;
			std::queue<UINT32> queue;
			User* user = nullptr;
			mUserReceiveEventQueueLock.lock();
			mUserReceiveEventQueue.swap(queue);
			mUserReceiveEventQueueLock.unlock();

			while (!queue.empty())
			{
				userIndex = queue.front(); queue.pop();
				user = mUserManager.GetUser(userIndex);
				if (user)
				{
					info = user->GetPacketInfo();
				}
				auto iter = mProcMap.find(info.packetId);
				if (iter != mProcMap.end())
				{
					(this->*(iter->second))(info.clientNum, info.pData, info.dataSize);
				}
			}
		}


		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}

void PacketManager::PushSystemInfo(UINT32 clientIndx, PACKET_ID systeminfo)
{
	PacketInfo packetInfo;
	packetInfo.clientNum = clientIndx;
	packetInfo.packetId = static_cast<UINT16>(systeminfo);

	mSystemInfoQueueLock.lock();
	mSystemInfoQueue.push(packetInfo);
	mSystemInfoQueueLock.unlock();
}
void PacketManager::PushReceiveData(UINT32 clientIndx, char* pData, UINT16 dwDataSize)
{
	User* user = nullptr;
	user = mUserManager.GetUser(clientIndx);
	if (user != nullptr)
	{
		user->PushLowData(pData, dwDataSize);
		mUserReceiveEventQueueLock.lock();
		mUserReceiveEventQueue.push(clientIndx);
		mUserReceiveEventQueueLock.unlock();
	}
}

void PacketManager::ProcessSystemConnect(UINT32 clientIndx, char* pData, UINT16 dataSize)
{
	User* user = nullptr;
	mUserManager.SetUserStatus(clientIndx, USER_STATUS_INFO::ON_LINE);
}
void PacketManager::ProcessSystemDisonnect(UINT32 clientIndx, char* pData, UINT16 dataSize)
{
	User* user = nullptr;
	mUserManager.SetUserStatus(clientIndx, USER_STATUS_INFO::OFF_LINE);
}
void PacketManager::ProcessReceiveChat(UINT32 clientIndx, char* pData, UINT16 dataSize)
{
	ChattingPacket chat;
	memcpy_s(&chat, sizeof(chat), pData, dataSize);
	mUserManager.SendToAllUser(clientIndx, (char*)&chat,sizeof(chat));
}
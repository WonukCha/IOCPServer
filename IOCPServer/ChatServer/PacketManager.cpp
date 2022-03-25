#include "PacketManager.h"

void PacketManager::Init(const UINT32 maxClientCount)
{

	mProcMap[static_cast<int>(PACKET_ID::SYSYEM_DISCONNECT)] = &PacketManager::ProcessSystemDisonnect;
	mProcMap[static_cast<int>(PACKET_ID::SYSYEM_CONNECT)]= &PacketManager::ProcessSystemConnect;

	mProcMap[static_cast<int>(PACKET_ID::LOGIN_REQUEST)] = &PacketManager::ProcessLogin;

	mProcMap[static_cast<int>(PACKET_ID::ALL_USER_CHAT_REQUEST)]= &PacketManager::ProcessAllUserChatMessage;

	mProcMap[static_cast<int>(PACKET_ID::ROOM_ENTER_REQUEST)] = &PacketManager::ProcessEnterRoom;
	mProcMap[static_cast<int>(PACKET_ID::ROOM_LEAVE_REQUEST)] = &PacketManager::ProcessLeaveRoom;
	mProcMap[static_cast<int>(PACKET_ID::ROOM_CHAT_REQUEST)] = &PacketManager::ProcessRoomChatMessage;


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
			PacketInfo* info = nullptr;
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
					if (info->packetId != static_cast<UINT16>(PACKET_ID::ALL_USER_CHAT_REQUEST))
					{
						continue;
					}
					auto iter = mProcMap.find(info->packetId);
					if (iter != mProcMap.end())
					{
						(this->*(iter->second))(info->clientNum, info->pData, info->dataSize);
					}
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}

void PacketManager::PushSystemInfo(UINT32 clientIndx, PACKET_ID systeminfo)
{
	PacketInfo systemInfo;
	systemInfo.clientNum = clientIndx;
	systemInfo.packetId = static_cast<UINT16>(systeminfo);

	mSystemInfoQueueLock.lock();
	mSystemInfoQueue.push(systemInfo);
	mSystemInfoQueueLock.unlock();
}
void PacketManager::PushReceiveData(UINT32 clientIndx, char* pData, UINT32 dataSize)
{
	User* user = nullptr;
	user = mUserManager.GetUser(clientIndx);
	if (user != nullptr)
	{
		if (user->PushLowData(pData, dataSize) == false)
		{
			std::cout<< "PushReceiveData Fail\r\n";
			return;
		}
		mUserReceiveEventQueueLock.lock();
		mUserReceiveEventQueue.push(clientIndx);
		mUserReceiveEventQueueLock.unlock();
	}
}
INT16 PacketManager::CompressPacket(void* pDest, rsize_t* pDestSize, PacketInfo* pPacketInfo, COMPRESS_TYPE compressType)
{
	INT16 result = -1;
	
	do
	{
		if (pPacketInfo == nullptr)
			break;

		PACKET_HEADER* pHeader = nullptr;
		pHeader = (PACKET_HEADER*)pPacketInfo->pData;

		if (pHeader == nullptr)
			break;
		if (pHeader->compressType != COMPRESS_TYPE::NONE)
			break;

		Bytef* pBeginData = (Bytef*)pHeader + sizeof(PACKET_HEADER);
		uLongf sourceLen = pHeader->packetSize - sizeof(PACKET_HEADER);
		uLongf destLen = sizeof(mCompressBuffer);
		switch (compressType)
		{
		case COMPRESS_TYPE::ZLIB:
		{
			result = compress(mCompressBuffer,&destLen, pBeginData, sourceLen);
			if (mCompressBuffer != pDest)
			{
				memcpy_s(pDest, *pDestSize, &mCompressBuffer, destLen);
			}
			*pDestSize = destLen;
		}
		break;
		default:
			result = 0;
			break;
		}
	} while (false);

	return result;
}

INT16 PacketManager::UncompressPacket(void* pDest, rsize_t* pDestSize, PacketInfo* pPacketInfo)
{
	INT16 result = -1;
	do
	{
		if (pPacketInfo == nullptr)
			break;

		PACKET_HEADER* pHeader = nullptr;
		pHeader = (PACKET_HEADER*)pPacketInfo->pData;

		if (pHeader == nullptr)
			break;
		if (pHeader->compressType == COMPRESS_TYPE::NONE)
			break;

		Bytef* pBeginData = (Bytef*)pHeader + sizeof(PACKET_HEADER);
		uLongf sourceLen = pHeader->packetSize - sizeof(PACKET_HEADER);
		uLongf destLen = sizeof(mCompressBuffer);
		switch (pHeader->compressType)
		{
		case COMPRESS_TYPE::ZLIB:
		{
			result = uncompress(mCompressBuffer, &destLen, pBeginData, sourceLen);
			if (mCompressBuffer != pDest)
			{
				memcpy_s(pDest, *pDestSize, &mCompressBuffer, destLen);
			}
			*pDestSize = destLen;
		}
		break;
		default:
			result = 0;
			break;
		}
	} while (false);

	return result;
}

void PacketManager::ProcessSystemConnect(UINT32 clientIndx, char* pData, UINT32 dataSize)
{
	mUserManager.SetUserStatus(clientIndx, USER_STATUS_INFO::CONNECT);
}
void PacketManager::ProcessSystemDisonnect(UINT32 clientIndx, char* pData, UINT32 dataSize)
{
	mUserManager.SetUserStatus(clientIndx, USER_STATUS_INFO::DISCONECT);
}
void PacketManager::ProcessAllUserChatMessage(UINT32 clientIndx, char* pData, UINT32 dataSize)
{
	//ChattingPacket chat;
	//memcpy_s(&chat, sizeof(chat), pData, dataSize);
	//chat.pakcetID = PACKET_ID::SERVER_TO_CLIENT_CHATTING;
	//chat.compressType = COMPRESS_TYPE::ZLIB;
	//chat.packetSize = sizeof(chat);
	//
	//uLongf destSize = sizeof(mCompressBuffer);
	//int zResult = compress((Bytef*)(mCompressBuffer + sizeof(PacketHeader)),&destSize,
	//	(Bytef*)(&chat.cName),sizeof(chat)-sizeof(PacketHeader));
	//
	//chat.packetSize = sizeof(PacketHeader) + destSize;
	//memcpy_s(mCompressBuffer,sizeof(mCompressBuffer), &chat, sizeof(PacketHeader));
	//
	//mUserManager.SendToAllUser(clientIndx, (char*)&mCompressBuffer, chat.packetSize);
	PACKET_HEADER packetHeader;
	packetHeader.compressType = COMPRESS_TYPE::NONE;
	packetHeader.packetSize = sizeof(PACKET_HEADER);
	packetHeader.pakcetID = PACKET_ID::ALL_USER_CHAT_RESPONSE;
	SendPacketFunc(clientIndx, (char*)&packetHeader, sizeof(packetHeader));

	PACKET_ID id = PACKET_ID::ALL_USER_CHAT_NOTIFY;
	memcpy(pData, &id,sizeof(id));
	mUserManager.SendToAllUser(clientIndx, (char*)pData, dataSize);
}

void PacketManager::ProcessLogin(UINT32 clientIndx, char* pData, UINT32 dataSize)
{

}

void PacketManager::ProcessEnterRoom(UINT32 clientIndx, char* pData, UINT32 dataSize)
{

}

void PacketManager::ProcessLeaveRoom(UINT32 clientIndx, char* pData, UINT32 dataSize)
{

}

void PacketManager::ProcessRoomChatMessage(UINT32 clientIndx, char* pData, UINT32 dataSize)
{

}

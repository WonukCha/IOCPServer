#include "PacketManager.h"

void PacketManager::Init(const UINT32 maxClientCount)
{

	mProcMap[static_cast<int>(PACKET_ID::SYSYEM_DISCONNECT)] = &PacketManager::ProcessSystemDisonnect;
	mProcMap[static_cast<int>(PACKET_ID::SYSYEM_CONNECT)]= &PacketManager::ProcessSystemConnect;

	mProcMap[static_cast<int>(PACKET_ID::LOGIN_REQUEST)] = &PacketManager::ProcessLogin;
	mProcMap[static_cast<int>(PACKET_ID::LOGOUT_REQUEST)] = &PacketManager::ProcessLogout;
	mProcMap[static_cast<int>(RedisTaskID::REDIS_RESPONSE_LOGIN)] = &PacketManager::ProcessLoginDBResult;

	mProcMap[static_cast<int>(PACKET_ID::ALL_USER_CHAT_REQUEST)]= &PacketManager::ProcessAllUserChatMessage;

	mProcMap[static_cast<int>(PACKET_ID::ROOM_ENTER_REQUEST)] = &PacketManager::ProcessEnterRoom;
	mProcMap[static_cast<int>(PACKET_ID::ROOM_LEAVE_REQUEST)] = &PacketManager::ProcessLeaveRoom;
	mProcMap[static_cast<int>(PACKET_ID::ROOM_CHAT_REQUEST)] = &PacketManager::ProcessRoomChatMessage;


	mUserManager.SendPacketFunc = SendPacketFunc;
	mRoomManager.SendPacketFunc = SendPacketFunc;
	mUserManager.Init(maxClientCount);
	mRoomManager.Init(0,10,100);
	mRedisManager.Init(REDIS_IP, REDIS_PORT,REDIS_PASSWORD);
}
void PacketManager::Run()
{
	mRedisManager.Run();

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
	mRedisManager.End();
}
void PacketManager::PacketProcess()
{
	bool Isidle = false;
	while (true)
	{
		if (mIsRunProcessThread == false)
			break;

		Isidle = true;
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
			Isidle = false; 
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
					//if (info->packetId != static_cast<UINT16>(PACKET_ID::ALL_USER_CHAT_REQUEST))
					//{
					//	continue;
					//}
					auto iter = mProcMap.find(info->packetId);
					if (iter != mProcMap.end())
					{
						(this->*(iter->second))(info->clientNum, info->pData, info->dataSize);
					}
				}
			}

		}
		if (!mRedisManager.EmptyResponseTeskQueue())
		{
			REDIS_RESPONSE_LOGIN redisResponse;
			std::queue<REDIS_RESPONSE_LOGIN> redisResponseQueue;
			mRedisManager.SwapResponseTeskQueue(&redisResponseQueue);

			while (!redisResponseQueue.empty())
			{
				redisResponse = redisResponseQueue.front(); redisResponseQueue.pop();
				auto iter = mProcMap.find((UINT16)redisResponse.RedisTaskId);
				if (iter != mProcMap.end())
				{
					(this->*(iter->second))(redisResponse.clientNum, (char*)&redisResponse, sizeof(redisResponse));
				}
			}
		}
		Isidle = false;
		if (Isidle)
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

		PACKET_HEADER* pHeader = (PACKET_HEADER*)pPacketInfo->pData;

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
	do
	{
		User* user = nullptr;
		user = mUserManager.GetUser(clientIndx);
		if (user == nullptr)
			break;
		mRoomManager.LeaveRoomUser(user->GetCurrentRoom(), user);
		
		mUserManager.SetUserStatus(clientIndx, USER_STATUS_INFO::DISCONECT);
	} while (false);
	
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
	
	//ALL_USER_CHAT_REQUEST alluUserChatRequest;
	//memcpy_s(&alluUserChatRequest, sizeof(alluUserChatRequest), pData, dataSize);
	//
	//alluUserChatRequest.pakcetID = PACKET_ID::ALL_USER_CHAT_NOTIFY;
	//alluUserChatRequest.compressType = COMPRESS_TYPE::ZLIB;
	//alluUserChatRequest.packetSize = sizeof(alluUserChatRequest);
	//
	//uLongf destSize = sizeof(mCompressBuffer);
	//int zResult = UncompressPacket((Bytef*)(mCompressBuffer + sizeof(PACKET_HEADER)),&destSize,
	//	(Bytef*)(&alluUserChatRequest.CheckCode),sizeof(alluUserChatRequest)-sizeof(PACKET_HEADER));
	//
	//UncompressPacket(alluUserChatRequest.CheckCode, sizeof(alluUserChatRequest) - sizeof(PACKET_HEADER), (PacketInfo*)pData);
	//
	//alluUserChatRequest.packetSize = sizeof(PACKET_HEADER) + destSize;
	//memcpy_s(mCompressBuffer,sizeof(mCompressBuffer), &alluUserChatRequest, sizeof(PACKET_HEADER));
	


	PACKET_HEADER packetHeader;
	packetHeader.compressType = COMPRESS_TYPE::NONE;
	packetHeader.packetSize = sizeof(PACKET_HEADER);
	packetHeader.pakcetID = PACKET_ID::ALL_USER_CHAT_RESPONSE;
	//SendPacketFunc(clientIndx, (char*)&packetHeader, sizeof(packetHeader));

	PACKET_ID id = PACKET_ID::ALL_USER_CHAT_NOTIFY;
	memcpy(pData, &id,sizeof(id));
	mUserManager.SendToAllUser(clientIndx, (char*)pData, dataSize);
}

void PacketManager::ProcessLogin(UINT32 clientIndx, char* pData, UINT32 dataSize)
{
	do
	{
		LOGIN_REQUEST* pLoginRequest = (LOGIN_REQUEST*)pData;
		if (pLoginRequest == nullptr)
			break;
		if (dataSize != sizeof(LOGIN_REQUEST))
			break;


		REDIS_REQUEST_LOGIN redisRequest;
		redisRequest.clientNum = clientIndx;
		redisRequest.dataSize = sizeof(REDIS_REQUEST_LOGIN);
		redisRequest.RedisTaskId = RedisTaskID::REDIS_REQUEST_LOGIN;
		redisRequest.tickCount = pLoginRequest->tickCount;
		redisRequest.pData = (char*)redisRequest.ID;
		memcpy_s(redisRequest.ID,sizeof(redisRequest.ID), pLoginRequest->ID,sizeof(pLoginRequest->ID));
		memcpy_s(redisRequest.PW, sizeof(redisRequest.PW), pLoginRequest->PW, sizeof(pLoginRequest->PW));
		mRedisManager.PushRequestTesk(redisRequest);

		//LOGIN_RESPONSE loginResponse;
		//loginResponse.TickCount = pLoginRequest->TickCount;
		//loginResponse.compressType = COMPRESS_TYPE::NONE;
		//loginResponse.pakcetID = PACKET_ID::LOGIN_RESPONSE;
		//loginResponse.packetSize = sizeof(LOGIN_RESPONSE);
		//
		//User* user = nullptr;
		//user = mUserManager.GetUser(clientIndx);
		//if (user == nullptr)
		//	break;
		//if (user->GetUserStatus() == USER_STATUS_INFO::CONNECT)
		//{
		//	user->SetUserStatus(USER_STATUS_INFO::LOBBY);
		//	loginResponse.Result = true;
		//}
		//else
		//{
		//	loginResponse.Result = false;
		//}
		//SendPacketFunc(clientIndx, (char*)&loginResponse, sizeof(loginResponse));
	} while (false);
}
void PacketManager::ProcessLogout(UINT32 clientIndx, char* pData, UINT32 dataSize)
{
	do
	{
		LOGOUT_REQUEST* pLoginRequest = (LOGOUT_REQUEST*)pData;
		if (pLoginRequest == nullptr)
			break;
		if (dataSize != sizeof(LOGOUT_REQUEST))
			break;
		LOGOUT_RESPONSE logoutResponse;
		logoutResponse.Result = false;
		User* user = nullptr;
		user = mUserManager.GetUser(clientIndx);
		if (user != nullptr && user->GetUserStatus() == USER_STATUS_INFO::LOBBY)
		{
			user->SetUserStatus(USER_STATUS_INFO::CONNECT);
			logoutResponse.Result = true;
		}

		logoutResponse.compressType = COMPRESS_TYPE::NONE;
		logoutResponse.packetSize = sizeof(logoutResponse);
		logoutResponse.pakcetID = PACKET_ID::LOGOUT_RESPONSE;
		logoutResponse.tickCount = pLoginRequest->tickCount;

		SendPacketFunc(clientIndx, (char*)&logoutResponse, sizeof(logoutResponse));
	} while (false);
}
void PacketManager::ProcessLoginDBResult(UINT32 clientIndx, char* pData, UINT32 dataSize)
{
	do
	{
		REDIS_RESPONSE_LOGIN* pRedisResponseLogin = (REDIS_RESPONSE_LOGIN*)pData;
		if (pRedisResponseLogin == nullptr)
			break;
		if (dataSize != sizeof(REDIS_RESPONSE_LOGIN))
			break;

		LOGIN_RESPONSE loginResponse;
		if (pRedisResponseLogin->Result == (UINT16)true)
		{
			loginResponse.Result = true;

			User* user = nullptr;
			user = mUserManager.GetUser(clientIndx);
			if (user != nullptr && user->GetUserStatus() == USER_STATUS_INFO::CONNECT)
			{
				user->SetUserStatus(USER_STATUS_INFO::LOBBY);
			}
		}
		
		loginResponse.compressType = COMPRESS_TYPE::NONE;
		loginResponse.packetSize = sizeof(loginResponse);
		loginResponse.pakcetID = PACKET_ID::LOGIN_RESPONSE;
		loginResponse.tickCount = pRedisResponseLogin->tickCount;

		SendPacketFunc(clientIndx, (char*)&loginResponse, sizeof(loginResponse));
	} while (false);
}

void PacketManager::ProcessEnterRoom(UINT32 clientIndx, char* pData, UINT32 dataSize)
{
	do
	{
		ROOM_ENTER_REQUEST* pRoomEnterRequest = (ROOM_ENTER_REQUEST*)pData;
		if (pRoomEnterRequest == nullptr)
			break;
		if (dataSize != sizeof(ROOM_ENTER_REQUEST))
			break;

		ROOM_ENTER_RESPONSE roomEnterResponse;
		roomEnterResponse.tickCount = pRoomEnterRequest->tickCount;
		roomEnterResponse.compressType = COMPRESS_TYPE::NONE;
		roomEnterResponse.pakcetID = PACKET_ID::ROOM_ENTER_RESPONSE;
		roomEnterResponse.packetSize = sizeof(ROOM_ENTER_RESPONSE);
		roomEnterResponse.Result = false;

		User* user = nullptr;
		user = mUserManager.GetUser(clientIndx);
		if (user == nullptr)
			break;
		if (user->GetUserStatus() == USER_STATUS_INFO::LOBBY)
		{
			roomEnterResponse.Result = mRoomManager.EnterRoomUser(pRoomEnterRequest->RoomNumber, user);
			if (roomEnterResponse.Result == (INT16)true)
			{
				user->EnterRoom(pRoomEnterRequest->RoomNumber);
				user->SetUserStatus(USER_STATUS_INFO::ROOM);
			}
		}
		SendPacketFunc(clientIndx, (char*)&roomEnterResponse, sizeof(roomEnterResponse));
	} while (false);
}

void PacketManager::ProcessLeaveRoom(UINT32 clientIndx, char* pData, UINT32 dataSize)
{
	do
	{
		ROOM_LEAVE_REQUEST* pRoomLeaveRequest = (ROOM_LEAVE_REQUEST*)pData;
		if (pRoomLeaveRequest == nullptr)
			break;
		if (dataSize != sizeof(ROOM_LEAVE_REQUEST))
			break;

		ROOM_LEAVE_RESPONSE roomLeaveResponse;
		roomLeaveResponse.tickCount = pRoomLeaveRequest->tickCount;
		roomLeaveResponse.compressType = COMPRESS_TYPE::NONE;
		roomLeaveResponse.pakcetID = PACKET_ID::ROOM_LEAVE_RESPONSE;
		roomLeaveResponse.packetSize = sizeof(ROOM_ENTER_RESPONSE);
		roomLeaveResponse.Result = false;
		User* user = nullptr;
		user = mUserManager.GetUser(clientIndx);
		if (user == nullptr)
			break;

		if (user->GetUserStatus() == USER_STATUS_INFO::ROOM)
		{
			roomLeaveResponse.Result = mRoomManager.LeaveRoomUser(pRoomLeaveRequest->RoomNumber, user);
			if (roomLeaveResponse.Result == (INT16)true)
			{
				user->SetUserStatus(USER_STATUS_INFO::LOBBY);
			}
		}
		SendPacketFunc(clientIndx, (char*)&roomLeaveResponse, sizeof(roomLeaveResponse));
	} while (false);
}

void PacketManager::ProcessRoomChatMessage(UINT32 clientIndx, char* pData, UINT32 dataSize)
{
	do
	{
		ROOM_CHAT_REQUEST* pRoomChatRequest = (ROOM_CHAT_REQUEST*)pData;
		if (pRoomChatRequest == nullptr)
			break;
		if (dataSize != sizeof(ROOM_CHAT_REQUEST))
			break;
		ROOM_CHAT_RESPONSE roomChatResponse;
		roomChatResponse.tickCount = pRoomChatRequest->tickCount;
		roomChatResponse.compressType = COMPRESS_TYPE::NONE;
		roomChatResponse.pakcetID = PACKET_ID::ROOM_CHAT_RESPONSE;
		roomChatResponse.packetSize = sizeof(ROOM_ENTER_RESPONSE);
		roomChatResponse.Result = false;

		User* user = nullptr;
		user = mUserManager.GetUser(clientIndx);
		if (user == nullptr)
			break;
		if (user->GetUserStatus() == USER_STATUS_INFO::ROOM)
		{
			roomChatResponse.Result = true;
			SendPacketFunc(clientIndx, (char*)&roomChatResponse, sizeof(roomChatResponse));
			user->GetID();
			ROOM_CHAT_NOTIFY roomChatNotify;
			roomChatNotify.tickCount = pRoomChatRequest->tickCount;
			roomChatNotify.compressType = COMPRESS_TYPE::NONE;
			roomChatNotify.pakcetID = PACKET_ID::ROOM_CHAT_NOTIFY;
			roomChatNotify.packetSize = sizeof(ROOM_CHAT_NOTIFY);

			memcpy_s(roomChatNotify.ID, sizeof(roomChatNotify.ID), user->GetID().c_str(), user->GetID().size());
			memcpy_s(roomChatNotify.Msg, sizeof(roomChatNotify.Msg), pRoomChatRequest->Msg, sizeof(pRoomChatRequest->Msg));

			Room* room = nullptr;
			room = mRoomManager.GetRoomByNumeber(user->GetCurrentRoom());
			if (room == nullptr)
				break;
			room->Nofify(clientIndx, (char*)&roomChatNotify, sizeof(roomChatNotify));
		}

	} while (false);
}

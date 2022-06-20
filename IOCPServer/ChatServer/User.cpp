#include "User.h"

void User::Init()
{
	mUserStatus = USER_STATUS_INFO::NONE;
	mID  = "";
	mRecvBuffer.Clear();
}
bool User::PushLowData(char* pData, UINT32 dwDataSize)
{
	return mRecvBuffer.PutData(pData, static_cast<size_t>(dwDataSize));
}
void User::SetUserStatus(USER_STATUS_INFO status)
{
	mUserStatus = status;
}
USER_STATUS_INFO User::GetUserStatus()
{
	return mUserStatus;
}
PacketInfo User::GetPacketInfo()
{
	PacketInfo stagePacketInfo;
	do
	{
		//memset(&mStagePacketInfo, 0, sizeof(mStagePacketInfo));

		if (mRecvBuffer.GetSize() < sizeof(PACKET_HEADER))
			break;

		PACKET_HEADER header;
		mRecvBuffer.GetData(&header,sizeof(header),rbuf_opt_e::RBUF_NO_CLEAR);

		//if (CheckPacketHeader(&header) == false)
		//	break;

		if (header.packetSize > mRecvBuffer.GetSize())
			break;

		//memset(&mStageBuffer, 0, sizeof(mStageBuffer));

		//switch (header.compressType)
		//{
		//	case COMPRESS_TYPE::ZLIB:
		//	{
		//		memset(&mCompressBuffer, 0, sizeof(mCompressBuffer));
		//
		//		mRecvBuffer.GetData(mStageBuffer, sizeof(PacketHeader), rbuf_opt_e::RBUF_CLEAR);
		//		mRecvBuffer.GetData(mCompressBuffer, header.packetSize - sizeof(PacketHeader), rbuf_opt_e::RBUF_CLEAR);
		//		uLongf destLen = sizeof(mStageBuffer) - sizeof(header);
		//		uLong size = (header.packetSize - sizeof(PacketHeader));
  		//		int result = uncompress((Bytef*)(mStageBuffer + sizeof(PacketHeader)), &destLen, (Bytef*)(mCompressBuffer), size);
		//		mStagePacketInfo.dataSize = sizeof(PacketHeader) + destLen;
		//	}
		//	break;
		//	case COMPRESS_TYPE::NONE:
		//	{
		//		mRecvBuffer.GetData(mStageBuffer, header.packetSize, rbuf_opt_e::RBUF_CLEAR);
		//		mStagePacketInfo.dataSize = header.packetSize;
		//	}
		//	break;
		//	default:
		//	{
		//	}
		//	break;
		//}

		mRecvBuffer.GetData(stageBuffer, header.packetSize, rbuf_opt_e::RBUF_CLEAR);
		stagePacketInfo.dataSize = header.packetSize;

		stagePacketInfo.clientNum = GetUserIndex();
		stagePacketInfo.dataSize = header.packetSize;
		stagePacketInfo.packetId = static_cast<UINT16>(header.pakcetID);
		stagePacketInfo.pData = stageBuffer;
	} while (false);

	return stagePacketInfo;
}
void User::SetUserIndex(UINT32 userIndex)
{
	mUserIndex = userIndex;
}
UINT32 User::GetUserIndex()
{
	return mUserIndex;
}
bool User::CheckPacketHeader(PACKET_HEADER* header)
{
	bool result = false;
	do
	{
		if (COMPRESS_TYPE::END <= header->compressType)
		{
			std::cout << "[ERROR] compressType error\r\n";
			break;
		}
		if (header->pakcetID == PACKET_ID::ALL_USER_CHAT_REQUEST)
		{
			if (header->packetSize > sizeof(ALL_USER_CHAT_REQUEST))
			{
				std::cout << "[ERROR] packetSize error\r\n";
				break;
			}
		}

		//switch (header->pakcetID)
		//{
		//case PACKET_ID::START:
		//	break;
		//case PACKET_ID::DISCONNECT:
		//	break;
		//case PACKET_ID::CONNECT:
		//	break;
		//case PACKET_ID::CLIENT_TO_SERVER_CHATTING:
		//	break;
		//case PACKET_ID::SERVER_TO_CLIENT_CHATTING:
		//	break;
		//case PACKET_ID::END:
		//	break;
		//default:
		//	break;
		//}
		result = true;
	} while (false);

	return result;
}
void User::EnterRoom(UINT16 roomNumber)
{
	mRoomNumber = roomNumber;
}
void User::LeaveRoom()
{
	mRoomNumber = UINT16_MAX;;
}
UINT16 User::GetCurrentRoom()
{
	return mRoomNumber;
}
void User::SetID(std::string id)
{
	mID = id;
}
std::string User::GetID()
{
	return mID;
}

#include "User.h"

void User::Init()
{
	mUserStatus = USER_STATUS_INFO::NONE;
	mId = "";
	mRecvBuffer.Clear();
	memset(mStageBuffer, 0, sizeof(mStageBuffer));
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
	PacketInfo info;

	do
	{
		if (mRecvBuffer.GetSize() < sizeof(PacketHeader))
			break;
		PacketHeader header;
		mRecvBuffer.GetData(&header,sizeof(header),rbuf_opt_e::RBUF_NO_CLEAR);

		if (header.unPacketSize > mRecvBuffer.GetSize())
			break;

		mRecvBuffer.GetData(mStageBuffer, header.unPacketSize, rbuf_opt_e::RBUF_CLEAR);

		info.clientNum = GetUserIndex();
		info.dataSize = header.unPacketSize;
		info.packetId = static_cast<UINT16>(header.pakcetID);
		info.pData = mStageBuffer;
	} while (false);

	return info;
}
void User::SetUserIndex(UINT32 userIndex)
{
	mUserIndex = userIndex;
}
UINT32 User::GetUserIndex()
{
	return mUserIndex;
}

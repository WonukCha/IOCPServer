#include "Room.h"
void Room::Init(UINT16 roomNumber, UINT16 maxUserCount)
{
	mRoomNumber = roomNumber;
	mMaxUserCount = maxUserCount;
}

bool Room::EnterUser(User* user)
{
	bool bResult = false;
	if(mCurUserCount < mMaxUserCount)
	{
		mUserList.push_back(user);
		mCurUserCount++;
		bResult = true;
	}
	return bResult;
}

bool Room::LeaveUser(User* user)
{
	mUserList.remove_if([user](User* pUser) {return pUser == user;});
	return true;
}

void Room::Nofify(UINT32 clientIndex, const char* pData, UINT32 dataSize)
{
	SendToAllUser(clientIndex, pData, dataSize);
}

void Room::SendToAllUser(UINT32 clientIndex, const char* pData, UINT32 dataSize)
{
	for (auto user : mUserList)
	{
		if (user->GetUserStatus() == USER_STATUS_INFO::DISCONECT)
			continue;

		SendPacketFunc(user->GetUserIndex(), (char*)pData, dataSize);
	}
}
#include "RoomManager.h"
void RoomManager::Init(UINT16 beginRoomNumber, UINT16 maxRoomCount, UINT16 maxRoomUserCount)
{
	mBeginRoomNumber = beginRoomNumber;
	mEndRoomNumber = beginRoomNumber + maxRoomCount;
	mRoomGroup.resize(maxRoomCount);
	for (UINT16 i = 0; i < maxRoomCount; i++)
	{
		mRoomGroup[i].Init(beginRoomNumber+1, maxRoomUserCount);
		mRoomGroup[i].SendPacketFunc = SendPacketFunc;
	}
}

bool RoomManager::EnterRoomUser(UINT16 roomNumber, User* user)
{
	bool result = false;

	do
	{
		Room* room = GetRoomByNumeber(roomNumber);
		if(room == nullptr)
			break;

		result = room->EnterUser(user);
	} while (false);

	return result;
}
bool RoomManager::LeaveRoomUser(UINT16 roomNumber, User* user)
{
	bool result = false;

	do
	{
		Room* room = GetRoomByNumeber(roomNumber);
		if (room == nullptr)
			break;

		result = room->LeaveUser(user);
	} while (false);

	return result;
}

Room* RoomManager::GetRoomByNumeber(UINT16 roomNumber)
{
	Room* result = nullptr;

	do
	{
		if (roomNumber < mBeginRoomNumber || mEndRoomNumber <= roomNumber)
			break;
		result = &mRoomGroup[roomNumber - mBeginRoomNumber];
	} while (false);
		
	return result;
}
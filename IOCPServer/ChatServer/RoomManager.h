#pragma once
#include<vector>

#include "Room.h"
class RoomManager
{
public:
	RoomManager() = default;
	virtual ~RoomManager() = default;

	void Init(UINT16 beginRoomNumber,UINT16 maxRoomCount,UINT16 maxRoomUserCount);

	bool EnterRoomUser(UINT16 roomNumber, User* user);
	bool LeaveRoomUser(UINT16 roomNumber, User* user);

	Room* GetRoomByNumeber(UINT16 roomNumber);
	std::function<void(UINT32, char*, UINT32)> SendPacketFunc;
private:
	std::vector<Room> mRoomGroup;

	UINT16 mBeginRoomNumber;
	UINT16 mEndRoomNumber;
	UINT16 mRoomCount;
};
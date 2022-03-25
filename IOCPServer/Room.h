#pragma once
#include<list>
#include<functional>


#include"User.h"
class Room
{
public:
	Room() = default;
	virtual ~Room() = default;
	void Init(UINT16 roomNumber, UINT16 maxUserCount);

	bool EnterUser(User* user);
	bool LeaveUser(User* user);

	std::function<void(UINT32, char*, UINT16)> SendPacketFunc;
private:
	void SendToAllUser(UINT32 clientIndex, const char* pData, UINT16 dataSize);

	std::list<User*> mUserList;
	UINT16 mRoomNumber = UINT16_MAX;
	UINT16 mCurUserCount = 0;
	UINT16 mMaxUserCount = 0;
};
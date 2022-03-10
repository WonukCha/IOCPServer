#pragma once
#include <vector>
#include <string>
#include <list>
#include <functional>

#include "User.h"

class UserManager
{
public:
	UserManager() = default;
	~UserManager() = default;

	void Init(const UINT32 maxUserCount);

	User* GetUser(const UINT32 userIndex);
	void SetUserStatus(const UINT32 userIndex, USER_STATUS_INFO info);
	void SendToAllUser(UINT32 clientIndex, const char* pData,UINT16 dataSize);

	std::function<void(UINT32, char*, UINT16)> SendPacketFunc;
private:
	bool InitUser(const UINT32 userIndex);

	std::vector<User> mUserList;
	std::list<User*> mOnLineUserList;

	User mNone;
};


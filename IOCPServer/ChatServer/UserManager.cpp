#include "UserManager.h"

void UserManager::Init(const UINT32 maxUserCount)
{
	std::vector<User> init(maxUserCount);
	mUserGroup.swap(init);

	for (UINT32 i = 0; i < maxUserCount; i++)
	{
		mUserGroup[i].Init();
		mUserGroup[i].SetUserIndex(i);
	}
}
User* UserManager::GetUser(const UINT32 userIndex)
{
	User* pResult = nullptr;
	if (mUserGroup.size() > userIndex)
	{
		pResult = &mUserGroup[userIndex];
	}
	return pResult;
}
bool UserManager::InitUser(const UINT32 userIndex)
{
	bool bResult = false;
	if (mUserGroup.size() > userIndex)
	{
		mUserGroup[userIndex].Init();
		bResult = true;
	}
	else
	{
		bResult = false;
	}
	return bResult;
}
void UserManager::SetUserStatus(const UINT32 userIndex, USER_STATUS_INFO info)
{
	if (mUserGroup.size() > userIndex)
	{
		mUserGroup[userIndex].Init();
		mUserGroup[userIndex].SetUserStatus(info);

		if (info == USER_STATUS_INFO::CONNECT)
		{
			mOnLineUserList.push_back(&mUserGroup[userIndex]);
		}
		if (info == USER_STATUS_INFO::DISCONECT)
		{
			mOnLineUserList.remove_if([userIndex = mUserGroup[userIndex].GetUserIndex()](User* pUser) {
				return userIndex == pUser->GetUserIndex();
			});
		}
	}
}
void UserManager::SendToAllUser(UINT32 clientIndex, const char* pData, UINT16 dataSize)
{
	for (auto user : mOnLineUserList)
	{
		if (user->GetUserStatus() == USER_STATUS_INFO::DISCONECT)
			continue;

		SendPacketFunc(user->GetUserIndex(), (char*)pData, dataSize);
	}
}
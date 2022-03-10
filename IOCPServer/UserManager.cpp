#include "UserManager.h"

void UserManager::Init(const UINT32 maxUserCount)
{
	std::vector<User> init(maxUserCount);
	mUserList.swap(init);

	for (UINT32 i = 0; i < maxUserCount; i++)
	{
		mUserList[i].Init();
		mUserList[i].SetUserIndex(i);
	}
}
User* UserManager::GetUser(const UINT32 userIndex)
{
	User* pResult = nullptr;
	if (mUserList.size() > userIndex)
	{
		pResult = &mUserList[userIndex];
	}
	return pResult;
}
bool UserManager::InitUser(const UINT32 userIndex)
{
	bool bResult = false;
	if (mUserList.size() > userIndex)
	{
		mUserList[userIndex].Init();
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
	if (mUserList.size() > userIndex)
	{
		mUserList[userIndex].Init();
		mUserList[userIndex].SetUserStatus(info);

		if (info == USER_STATUS_INFO::ON_LINE)
		{
			mOnLineUserList.push_back(&mUserList[userIndex]);
		}
		if (info == USER_STATUS_INFO::OFF_LINE)
		{
			mOnLineUserList.remove_if([userIndex = mUserList[userIndex].GetUserIndex()](User* pUser) {
				return userIndex == pUser->GetUserIndex();
			});
		}
	}
}
void UserManager::SendToAllUser(UINT32 clientIndex, const char* pData, UINT16 dataSize)
{
	for (auto user : mOnLineUserList)
	{
		if (user->GetUserIndex() == clientIndex)
			continue;

		if (user->GetUserStatus() == USER_STATUS_INFO::OFF_LINE)
			continue;

		SendPacketFunc(user->GetUserIndex(), (char*)pData, dataSize);
	}
}


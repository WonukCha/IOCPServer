#include "ClientInfo.h"

ClientInfo::ClientInfo()
{

}
ClientInfo::~ClientInfo()
{

}

void ClientInfo::Init(const unsigned int idex, const HANDLE iocpHandle)
{
	mClinetIndex = idex;
	mIocpHandle = iocpHandle;
}
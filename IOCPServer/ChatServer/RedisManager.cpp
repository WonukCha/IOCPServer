#include "RedisManager.h"

void RedisManager::Init(const std::string& host, const uint16_t port, const std::string& password)
{
	End();
	mRedisConn.connect(host,port,password);
}

void RedisManager::End()
{
	mIsRun = false;
	if (mRedisThread.joinable())
	{
		mRedisThread.join();
	}
}
void RedisManager::Run()
{
	mIsRun = true;
	mRedisThread = std::thread([this]() {this->RedisProcess(); });
}
bool RedisManager::IsConneced()
{
	return mRedisConn.isConneced();
}
void RedisManager::RedisProcess()
{
	bool IsIdle = false;

	REDIS_REQUEST_LOGIN redisRequest = {0,};
	REDIS_RESPONSE_LOGIN redisResponse = { 0, };
	std::string key;
	std::string value;
	while (true)
	{
		IsIdle = true;

		if (mIsRun == false)
			break;
		do
		{
			if (mRedisConn.isConneced() == false)
				break;
			if (mRedisRequestQueue.empty())
				break;
			mRedisRequestMutex.lock();
			redisRequest = std::move(mRedisRequestQueue.front()); mRedisRequestQueue.pop();
			mRedisRequestMutex.unlock();

			if (redisRequest.RedisTaskId != RedisTaskID::REDIS_REQUEST_LOGIN)
				break;
			
			redisResponse.Result = false;
			if (mRedisConn.get(redisRequest.ID, value) == false)
			{
				if (value.compare(redisRequest.PW) == 0)
				{
					redisResponse.Result = true;
				}
			}
			redisResponse.clientNum = redisRequest.clientNum;
			redisResponse.dataSize = sizeof(redisResponse);
			redisResponse.RedisTaskId = RedisTaskID::REDIS_RESPONSE_LOGIN;
			redisResponse.tickCount = redisRequest.tickCount;
			redisResponse.pData = (char*)redisResponse.Result;

			mRedisResponseMutex.lock();
			mRedisResponseQueue.push(std::move(redisResponse));
			mRedisResponseMutex.unlock();
			IsIdle = false;
		} while (false);
		
		if (IsIdle)
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}


}
bool RedisManager::PushRequestTesk(REDIS_REQUEST_LOGIN& redisRequest)
{
	bool result = false;
	if (MAX_REDIS_QUEUE_SIZE > mRedisRequestQueue.size())
	{
		mRedisRequestMutex.lock();
		mRedisRequestQueue.push(std::move(redisRequest));
		mRedisRequestMutex.unlock();

		result = true;
	}
	return result;
}
bool RedisManager::PopResponseTesk(REDIS_RESPONSE_LOGIN* redisResponse)
{
	bool result = false;
	if (mRedisResponseQueue.empty() == false)
	{
		mRedisResponseMutex.lock();
		*redisResponse = std::move(mRedisResponseQueue.front()); mRedisResponseQueue.pop();
		mRedisResponseMutex.unlock();
		result = true;
	}
	return result;
}
bool RedisManager::SwapResponseTeskQueue(OUT std::queue<REDIS_RESPONSE_LOGIN>* redisResponseQueue)
{
	bool result = false;
	do
	{
		if (redisResponseQueue == nullptr)
			break;
		if (mRedisResponseQueue.empty())
			break;
		mRedisResponseMutex.lock();
		mRedisResponseQueue.swap(*redisResponseQueue);
		mRedisResponseMutex.unlock();

	} while (false);
	return result;
}
bool RedisManager::EmptyResponseTeskQueue()
{
	return mRedisResponseQueue.empty();
}
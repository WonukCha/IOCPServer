#pragma once
#include <thread>
#include <string>
#include <queue>
#include <mutex>

#include "CRedisConn.h"
#include "RedisDefine.h"

constexpr UINT16 MAX_REDIS_QUEUE_SIZE = 10000;

class RedisManager
{
public:
	RedisManager() = default;
	~RedisManager() = default;

	void Init(const std::string& host, const uint16_t port, const std::string& password);
	void End();
	void Run();
	bool IsConneced();

	bool PushRequestTesk(REDIS_REQUEST_LOGIN& redisRequest);
	bool PopResponseTesk(REDIS_RESPONSE_LOGIN* redisResponse);
	bool SwapResponseTeskQueue(OUT std::queue<REDIS_RESPONSE_LOGIN>* redisResponseQueue);
	bool EmptyResponseTeskQueue();
private:
	RedisCpp::CRedisConn mRedisConn;

	void RedisProcess();

	bool mIsRun = false;
	std::thread mRedisThread;

	std::mutex mRedisRequestMutex;
	std::queue<REDIS_REQUEST_LOGIN>mRedisRequestQueue;

	std::mutex mRedisResponseMutex;
	std::queue<REDIS_RESPONSE_LOGIN>mRedisResponseQueue;
};


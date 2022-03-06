#pragma once
#include<thread>
#include <unordered_map>

class PacketManager
{
public:
	PacketManager() = default;
	~PacketManager() = default;

	void Init(const unsigned int maxClientCount);
	void Run();
	void End();

private:
	void PacketProcess();

	bool mIsRunProcessThread;
	std::thread mProcessThread;

	std::unordered_map<>;
};


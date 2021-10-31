#include <iostream>
#include <string>
#include "IOCPServer.h"

using namespace std;

//constexpr unsigned int threadCount = 3;
#define SERVER_PORT 10000

#define MAX_THREAD_COUNT 3

#define MAX_CLIENT_COUNT 10


int main(void)
{
	IOCPServer server;

	server.Init(MAX_THREAD_COUNT);
	server.BindListen(SERVER_PORT);
	server.StartServer(MAX_CLIENT_COUNT);

	printf("아무 키나 누를 때까지 대기합니다\n");
	while (true)
	{
		std::string inputCmd;
		std::getline(std::cin, inputCmd);

		if (inputCmd == "quit")
		{
			break;
		}
	}

	server.DestroyThread();
	
	return 0;
}
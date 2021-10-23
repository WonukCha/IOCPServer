#include <iostream>
#include "IOCPServer.h"

using namespace std;

//constexpr unsigned int threadCount = 3;
#define MAX_THREAD_COUNT 3
#define SERVER_PORT 10000

int main(void)
{
	IOCPServer server;

	server.Init(MAX_THREAD_COUNT);
	server.BindListen(SERVER_PORT);
	
	return 0;
}
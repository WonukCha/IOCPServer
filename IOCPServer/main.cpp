#include <iostream>
#include <string>
#include "ChatServer.h"

#define SERVER_PORT 10000
#define MAX_THREAD_COUNT 10
#define MAX_CLIENT_COUNT 100

int main(void)
{
	ChatServer chatServer;
	chatServer.Init(MAX_THREAD_COUNT);
	chatServer.BindListen(SERVER_PORT);
	chatServer.Run(MAX_CLIENT_COUNT);

	return 0;
}
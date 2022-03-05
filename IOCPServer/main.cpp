#include <iostream>
#include <string>
#include "EchoServer.h"

using namespace std;

//constexpr unsigned int threadCount = 3;
#define SERVER_PORT 10000

#define MAX_THREAD_COUNT 10

#define MAX_CLIENT_COUNT 1000

int main(void)
{
	EchoServer server;

	server.Init(MAX_THREAD_COUNT);
	server.BindListen(SERVER_PORT);
	server.StartServer(MAX_CLIENT_COUNT);

	cout << "�ƹ� Ű�� ���� ������ ����մϴ�\r\n";
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
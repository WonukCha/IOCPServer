#include <iostream>
#include <string>
#include "ChatServer.h"

#define SERVER_PORT 6000
#define MAX_THREAD_COUNT 5
#define MAX_CLIENT_COUNT 1001

int main(void)
{
	ChatServer chatServer;
	chatServer.Init(MAX_THREAD_COUNT);
	chatServer.BindListen(SERVER_PORT);
	chatServer.Run(MAX_CLIENT_COUNT);

	printf("�ƹ� Ű�� ���� ������ ����մϴ�\n");
	while (true)
	{
		std::string inputCmd;
		std::getline(std::cin, inputCmd);

		if (inputCmd == "quit")
		{
			break; 
		}
	}
	chatServer.End();
	return 0;
}
#include "EchoServer.h"
void EchoServer::OnConnect(unsigned int clientIndx)
{
	std::cout << "Client : "<< clientIndx << ", OnConnect\r\n";

}
void EchoServer::OnClose(unsigned int clientIndx)
{
	std::cout << "Client : " << clientIndx << ", OnClose\r\n";
}
void EchoServer::OnReceive(unsigned int clientIndx, RingbufferLock* pRingbuf)
{
	size_t bufSize = pRingbuf->GetSize();
	char* buf = new char[bufSize + 1];
	buf[bufSize] = '\0';

	std::string str;
	str.resize(bufSize);
	pRingbuf->GetData(buf, bufSize, rbuf_opt_e::RBUF_CLEAR);
	str = buf;
	std::cout << "Client : " << clientIndx << ", OnReceive : " << str << "\r\n";
	SendMsg(clientIndx,bufSize, buf);

	delete[] buf;
	buf = nullptr;
}
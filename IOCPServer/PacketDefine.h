#pragma once
#include <basetsd.h>

constexpr UINT16 USER_BUFFER_SIZE = 1204;
constexpr UINT16 CHAT_SIZE = 512;
constexpr UINT16 NAME_SIZE = 255;


enum class USER_STATUS_INFO : UINT8
{
	NONE = 0,
	OFF_LINE,
	ON_LINE
};

enum class PACKET_ID : UINT16
{
	PACKET_ID_DISCONNECT = 0,
	PACKET_ID_CONNECT,

	PACKET_ID_CLIENT_TO_SERVER_CHATTING = 100,
	PACKET_ID_SERVER_TO_CLIENT_CHATTING,
	PACKET_ID_END

};

struct PacketInfo
{
	UINT32 clientNum = 0;
	UINT16 packetId = 0;
	UINT16 dataSize = 0;
	char* pData = nullptr;
};

#pragma pack(push, 1)

struct PacketHeader
{
	PACKET_ID pakcetID;
	UINT16	unPacketSize;
};

struct ChattingPacket : public PacketHeader
{
	char cName[NAME_SIZE] = { '\0', };
	char cChat[CHAT_SIZE] = {'\0',};
};

#pragma pack(pop)

#pragma once
#include <basetsd.h>
#include <map>


constexpr UINT16 USER_BUFFER_SIZE = 1204;
constexpr UINT16 CHAT_SIZE = 256;
constexpr UINT16 NAME_SIZE = 256;

enum class COMPRESS_TYPE : UINT8
{
	NONE = 0,
	ZLIB,
	END
};

enum class USER_STATUS_INFO : UINT8
{
	NONE = 0,
	OFF_LINE,
	ON_LINE
};

enum class PACKET_ID : UINT16
{
	START,
	DISCONNECT = 100,
	CONNECT,

	CLIENT_TO_SERVER_CHATTING = 200,
	SERVER_TO_CLIENT_CHATTING,
	END
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
	UINT16	packetSize;
	COMPRESS_TYPE	compressType;
};

struct ChattingPacket : public PacketHeader
{
	char cName[NAME_SIZE] = { '\0', };
	char cChat[CHAT_SIZE] = { '\0', };
	unsigned __int64 ulTickCount = 0;
};

#pragma pack(pop)

#pragma once
#include <basetsd.h>
#include <map>


constexpr UINT16 CHAT_SIZE = 255 + 1;
constexpr UINT16 NAME_SIZE = 255 + 1;
constexpr UINT16 ID_SIZE = 255 + 1;
constexpr UINT16 PW_SIZE = 255 + 1;

enum class COMPRESS_TYPE : UINT8
{
	NONE = 0,
	ZLIB,
	END
};

enum class USER_STATUS_INFO : UINT8
{
	NONE = 0,
	DISCONECT,
	CONNECT,
	LOBBY,
	ROOM,

};

enum class PACKET_ID : UINT16
{
	NONE,
	SYSYEM_DISCONNECT = 100,
	SYSYEM_CONNECT,

	LOGIN_REQUEST = 200,
	LOGIN_RESPONSE,

	ALL_USER_CHAT_REQUEST = 300,
	ALL_USER_CHAT_RESPONSE,
	ALL_USER_CHAT_NOTIFY,

	ROOM_ENTER_REQUEST,
	ROOM_ENTER_RESPONSE,
	  
	ROOM_LEAVE_REQUEST,
	ROOM_LEAVE_RESPONSE,

	ROOM_CHAT_REQUEST,
	ROOM_CHAT_RESPONSE,
	ROOM_CHAT_NOTIFY,
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

struct PACKET_HEADER
{
	PACKET_ID pakcetID = PACKET_ID::NONE;
	UINT16	packetSize = 0;
	COMPRESS_TYPE	compressType = COMPRESS_TYPE::NONE;
	unsigned __int64 TickCount = 0;
};

struct LOGIN_REQUEST : public PACKET_HEADER
{
	char ID[ID_SIZE] = { '\0', };
	char PW[PW_SIZE] = { '\0', };
};
struct LOGIN_RESPONSE : public PACKET_HEADER
{
	INT16 Result;
};

typedef struct ALL_USER_CHAT_REQUEST : public PACKET_HEADER
{
	char CheckCode[NAME_SIZE] = { '\0', };
	char Msg[CHAT_SIZE] = { '\0', };
}ALL_USER_CHAT_NOTIFY;

struct ALL_USER_CHAT_RESPONSE : public PACKET_HEADER
{

};

struct ROOM_ENTER_REQUEST : public PACKET_HEADER
{
	UINT16 RoomNumber;
};

struct ROOM_ENTER_RESPONSE : public PACKET_HEADER
{
	INT16 Result;
};

struct ROOM_LEAVE_REQUEST : public PACKET_HEADER
{
	UINT16 RoomNumber;
};

struct ROOM_LEAVE_RESPONSE : public PACKET_HEADER
{
	INT16 Result;
};

struct ROOM_CHAT_REQUEST : public PACKET_HEADER
{
	char Msg[CHAT_SIZE] = { '\0', };
};

struct ROOM_CHAT_RESPONSE : public PACKET_HEADER
{
	INT16 Result;
};

struct ROOM_CHAT_NOTIFY : public PACKET_HEADER
{
	char ID[ID_SIZE] = { '\0', };
	char Msg[CHAT_SIZE] = { '\0', };
};
#pragma pack(pop)

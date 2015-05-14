#ifndef __COMMON_PROTOCOL_H__
#define __COMMON_PROTOCOL_H__

#include <stdint.h>

typedef enum PacketType PacketType;
enum PacketType {
	PACKET_TYPE_LOBBY,
	PACKET_TYPE_OBJECT,
};

typedef struct PacketLobby PacketLobby;
struct PacketLobby {
	uint32_t type;
	
	uint32_t begin;
};

typedef struct PacketObject PacketObject;
struct PacketObject {
	uint32_t type;
	
	uint32_t id;
	double x;
	double y;
	double angle;
};



#endif

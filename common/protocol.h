#ifndef __COMMON_PROTOCOL_H__
#define __COMMON_PROTOCOL_H__

#include <stdint.h>

typedef enum PacketType PacketType;
enum PacketType {
	PACKET_TYPE_LOBBY,
	PACKET_TYPE_SETUP,
	PACKET_TYPE_SETUP_OBJECT,
	PACKET_TYPE_OBJECT,
};

typedef struct PacketLobby PacketLobby;
struct PacketLobby {
	uint32_t type;
	
	uint32_t begin;
};

typedef struct PacketSetup PacketSetup;
struct PacketSetup {
	uint32_t type;
	
	uint32_t objects;
};

typedef struct PacketSetupObject PacketSetupObject;
struct PacketSetupObject {
	uint32_t type;
	
	uint32_t id;
	uint32_t sprite;
};

typedef struct PacketObject PacketObject;
struct PacketObject {
	uint32_t type;
	
	uint32_t id;
	double x;
	double y;
	double angle;
};

typedef struct PacketClient PacketClient;
struct PacketClient {
	uint32_t type;
	
	double angle;
	struct {
		uint32_t forward : 1;
		uint32_t backward : 1;
		uint32_t shoot : 1;
		uint32_t beam : 1;
	} button;
};

typedef union Packet Packet;
union Packet {
	uint32_t type;
	PacketLobby lobby;
	PacketSetup setup;
	PacketSetupObject setup_object;
	PacketObject object;
	PacketClient client;
};

#endif

#ifndef __COMMON_PROTOCOL_H__
#define __COMMON_PROTOCOL_H__

#include <stdint.h>

#define NAME_LEN_MAX 16

typedef enum PacketType PacketType;
enum PacketType {
	PACKET_TYPE_LOBBY,
	PACKET_TYPE_SETUP,
	PACKET_TYPE_SETUP_OBJECT,
	PACKET_TYPE_OBJECT,
	PACKET_TYPE_CLIENT,
	PACKET_TYPE_PRE_SIMULATION,
	PACKET_TYPE_PLAYER,
	PACKET_TYPE_AUX_PLAYER,
};

typedef struct PacketLobby PacketLobby;
struct PacketLobby {
	uint32_t type;
	
	uint32_t begin;
	char name[NAME_LEN_MAX];
};

typedef struct PacketSetup PacketSetup;
struct PacketSetup {
	uint32_t type;
	
	uint32_t id;
	uint32_t objects;
//	double width;
	double map_width;
	uint32_t pre_simulations;
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

typedef struct PacketPreSimulate PacketPreSimulate;
struct PacketPreSimulate {
	uint32_t type;
	
	uint32_t id;
	double x;
	double y;
};

typedef struct PacketPlayer PacketPlayer;
struct PacketPlayer {
	uint32_t type;
	
	double velocity;
	double accel;
	double energy;
};


typedef struct PacketAuxPlayer PacketAuxPlayer;
struct PacketAuxPlayer {
	uint32_t type;
	uint32_t id;
	double tractor_beam;
	uint8_t score[8];
};

typedef union Packet Packet;
union Packet {
	uint32_t type;
	PacketLobby lobby;
	PacketSetup setup;
	PacketSetupObject setup_object;
	PacketObject object;
	PacketClient client;
	PacketPlayer player;
	PacketPreSimulate simul;
	PacketAuxPlayer auxplayer;
};

#endif

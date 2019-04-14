#pragma once
#include <string>

#define MAX_PACKET_SIZE 1000000

enum PacketTypes {

	INIT_CONNECTION = 0,
	ACTION_EVENT = 1,
	FORWARD_EVENT = 2,
	BACKWARD_EVENT = 3,
	LEFT_EVENT = 4,
	RIGHT_EVENT = 5,

};

struct Packet {

	unsigned int packet_type;
	//std::string packet_id;
	char databuf;

	void serialize(char * data) {
		memcpy(data, this, sizeof(Packet));
	}

	void deserialize(char * data) {
		memcpy(this, data, sizeof(Packet));
	}
};
#pragma once
#include <string.h>
#include <iostream>

#define MAX_PACKET_SIZE 1000000

enum PacketTypes {

	INIT_CONNECTION = 0,

	ACTION_EVENT = 1,

};

struct Packet {

	unsigned int packet_type;
	char databuf;

	void serialize(char * data) {
		memcpy(data, this, sizeof(Packet));
		//std::cout << data << std::endl;
	}

	void deserialize(char * data) {
		memcpy(this, data, sizeof(Packet));
		//std::cout << data << std::endl;

	}
};
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
	char * data;

	void serialize(char * data) {
		std::cout << data << std::endl;
		memcpy(data, this, sizeof(Packet));
	}

	void deserialize(char * data) {
		std::cout << data << std::endl;
		memcpy(this, data, sizeof(Packet));
	}
};
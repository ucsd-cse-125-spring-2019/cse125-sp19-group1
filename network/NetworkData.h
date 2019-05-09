#pragma once
#include <string>

#include <string.h>
#include <iostream>

#define MAX_PACKET_SIZE 1000000

enum PacketTypes {

	INIT_CONNECTION = 0,
	ACTION_EVENT = 1,
	FORWARD_EVENT = 2,
	BACKWARD_EVENT = 3,
	LEFT_EVENT = 4,
	RIGHT_EVENT = 5,
	INTERACT_EVENT = 6,
	RELEASE_EVENT = 7,
	DROP_EVENT = 8,
	HIDE_EVENT = 9
};

struct Packet {

	unsigned int packet_type;

	char databuf;

	//std::string packet_id;

	char id [9];


	void serialize(char * data) {
		memcpy(data, this, sizeof(Packet));
		//std::cout << data << std::endl;
	}

	void deserialize(char * data) {
		memcpy(this, data, sizeof(Packet));
		//std::cout << data << std::endl;

	}
};
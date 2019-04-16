#pragma once
<<<<<<< HEAD
#include <string>
=======
#include <string.h>
#include <iostream>
>>>>>>> b957bcb1eb9c579f8b2ea435f62544a21d8ea080

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
<<<<<<< HEAD
	//std::string packet_id;
	char databuf;
=======
	char * data;
>>>>>>> b957bcb1eb9c579f8b2ea435f62544a21d8ea080

	void serialize(char * data) {
		std::cout << data << std::endl;
		memcpy(data, this, sizeof(Packet));
	}

	void deserialize(char * data) {
		std::cout << data << std::endl;
		memcpy(this, data, sizeof(Packet));
	}
};
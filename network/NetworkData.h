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
	HIDE_EVENT = 9,
	RESET_EVENT = 10,
	POWERUP_EVENT = 11,
	SELECT_EVENT,
	READY_EVENT,
	START_EVENT,
	SELECT0_EVENT,
	SELECT1_EVENT,
	SELECT2_EVENT,
	SELECT3_EVENT,
	SELECT4_EVENT,
	OPEN_ALL_BOXES_EVENT,
	INCREMENT_ANGER_EVENT,
	UNLOCK_ALL_GATES_EVENT,
	DONE_LOADING_EVENT,
	SET_DONE_LOADING_EVENT,
	GO_TO_CREDITS_EVENT,
	PLAYER_DASH_EVENT,
	REMOVE_WALLS_EVENT,
};

struct Packet {

	unsigned int packet_type;

	char databuf;

	//std::string packet_id;

	int id;
	int selectionNum;

	void serialize(char * data) {
		memcpy(data, this, sizeof(Packet));
		//std::cout << data << std::endl;
	}

	void deserialize(char * data) {
		memcpy(this, data, sizeof(Packet));
		//std::cout << data << std::endl;

	}
};
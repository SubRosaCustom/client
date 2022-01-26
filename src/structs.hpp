#pragma once

#include <errno.h>
#include "utils.hpp"

struct Vector3 {
	float x, y, z;

	Vector3(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	};
};

static constexpr int maxServerListEntries = 32;

// 0x5c bytes
struct ServerListEntry {
	int versionMajor;
	int versionMinor; // 4
	int networkVersion; // 8
	char name[32]; // c
	PAD(0x30 - 0xc - 32);
	unsigned char ip[4]; // 30
	unsigned int port; // 34
	PAD(0x40 - 0x34 - 4);
	int numOfPlayers; // 40
	int unk0; // 44
	int maxNumOfPlayers;  // 48
	int ping; // 4c
	int gameType; // 50
	PAD(0x5c - 0x50 - 4);
};

// 0x9c bytes
struct Mouse {
	// possibly bound x 
	int unk0;
	// possibly bound y
	int unk1; // 4
	int deltaX; // 8
	int deltaY; // c
	PAD(0x90 - 0xc - 4);
	int isLeftMouseDown; // 90
	int isRightMouseDown; // 94
	int mouseInputs; // 9c
};

// 0x98 bytes
struct ChatMessage {
	int type;
	int unused;
	int timer;
	int otherPlayerID;
	int playerID;
	int volume;
	char message[128];
};
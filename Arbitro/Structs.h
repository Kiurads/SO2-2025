#pragma once

#ifndef STRUCTS_H
#define STRUCTS_H

#include <windows.h>
#include <tchar.h>

#define MAX_PLAYERS 20
#define BROADCAST_MESSAGE_SIZE 256

typedef struct {
	HANDLE hEvent;
	TCHAR message[BROADCAST_MESSAGE_SIZE];
} BroadcastData;

typedef struct Player Player;

struct Player {
	BOOL active;
	TCHAR name[50];
	int score;
	HANDLE hPipe;
	HANDLE hPipeBroadcast;
	HANDLE hThread;
	BroadcastData* broadcastData;
	Player* players;
};

typedef struct {
	Player players[20];
	HKEY hKey;
	int maxletras;
	int ritmo;
	BroadcastData *broadcastData;
} ServerData;

#endif // STRUCTS_H
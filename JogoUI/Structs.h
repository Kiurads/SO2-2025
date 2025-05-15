#pragma once
#include <windows.h>

typedef struct {
	HANDLE hPipe;
	BOOL running;
} Broadcast;
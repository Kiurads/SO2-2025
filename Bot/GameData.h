#pragma once
#include "Includes.h"

#define MAX_NAME_LENGTH 100

typedef struct {
	TCHAR username[MAX_NAME_LENGTH];
	HANDLE hPipe;
};
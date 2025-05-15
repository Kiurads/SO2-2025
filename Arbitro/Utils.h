#pragma once
#include <windows.h>
#include <tchar.h>

#define NO_COMMAND_CODE 0
#define GET_SCORE_COMMAND_CODE 1
#define GET_PLAYERS_COMMAND_CODE 2
#define EXIT_COMMAND_CODE 3

int isCommand(TCHAR* command);
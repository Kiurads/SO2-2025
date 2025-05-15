#include "Utils.h"
#include <stdio.h>

int isCommand(TCHAR* command)
{
	if (_tcscmp(command, TEXT(":pont")) == 0)
	{
		_tprintf(TEXT("[IsCommand] Comando :pont recebido.\n"));
		return GET_SCORE_COMMAND_CODE;
	}
	else if (_tcscmp(command, TEXT(":jogs")) == 0)
	{
		_tprintf(TEXT("[IsCommand] Comando :jogs recebido.\n"));
		return GET_PLAYERS_COMMAND_CODE;
	}
	else if (_tcscmp(command, TEXT(":sair")) == 0)
	{
		_tprintf(TEXT("[IsCommand] Comando :sair recebido.\n"));
		return EXIT_COMMAND_CODE;
	}
	else
	{
		return NO_COMMAND_CODE;
	}
}
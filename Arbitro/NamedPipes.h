#pragma once

#ifndef NAMEDPIPES_H
#define NAMEDPIPES_H

#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <stdio.h>

#include "Structs.h"

#define PIPE_NAME TEXT("\\\\.\\pipe\\MainPipe")
#define BROADCAST_PIPE_NAME TEXT("\\\\.\\pipe\\BroadcastPipe")
#define BUFSIZE 512

#define NO_FREE_PLAYER_ERROR_CODE -1
#define USERNAME_EXISTS_ERROR_CODE -2

#define NO_FREE_PLAYER_ERROR_MESSAGE TEXT("Não há posições livres.")
#define USERNAME_EXISTS_ERROR_MESSAGE TEXT("Nome de usuário já existe.")
#define SUCCESSFUL_CONNECTION_MESSAGE TEXT("Conexão estabelecida com sucesso.")

BOOL createClientNamedPipe(HANDLE* hPipe);
BOOL createBroadcastNamedPipe(HANDLE* hPipe);
int validatePlayer(ServerData* serverData, TCHAR *username);
DWORD WINAPI acceptConnections(LPVOID lpvParam);
DWORD WINAPI manageClientThread(LPVOID lpvParam);

#endif // NAMEDPIPES_H
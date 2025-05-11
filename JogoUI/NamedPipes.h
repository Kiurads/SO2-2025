#pragma once

#ifndef NAMEDPIPES_H
#define NAMEDPIPES_H

#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <stdio.h>

#define PIPE_NAME TEXT("\\\\.\\pipe\\MainPipe")
#define BROADCAST_PIPE_NAME TEXT("\\\\.\\pipe\\BroadcastPipe")
#define BUFSIZE 512

#define NO_FREE_PLAYER_ERROR_MESSAGE TEXT("Não há posições livres.")
#define USERNAME_EXISTS_ERROR_MESSAGE TEXT("Nome de usuário já existe.")

BOOL openNamedPipe(HANDLE* hPipe);
BOOL openBroadcastNamedPipe(HANDLE* hPipe);
BOOL registerOnServer(HANDLE hPipe, TCHAR* username);
DWORD WINAPI readServerMessages(LPVOID lpvParam);

#endif // NAMEDPIPES_H
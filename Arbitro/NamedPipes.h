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

BOOL createClientNamedPipe(HANDLE* hPipe);
BOOL createBroadcastNamedPipe(HANDLE* hPipe);
DWORD WINAPI acceptConnections(LPVOID lpvParam);
DWORD WINAPI manageClientThread(LPVOID lpvParam);

#endif // NAMEDPIPES_H
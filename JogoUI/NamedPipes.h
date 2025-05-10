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

BOOL openNamedPipe(HANDLE* hPipe);
BOOL openBroadcastNamedPipe(HANDLE* hPipe);
BOOL testNamedPipe(HANDLE* hPipe);
DWORD WINAPI readServerMessages(LPVOID lpvParam);

#endif // NAMEDPIPES_H
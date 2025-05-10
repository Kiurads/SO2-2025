#pragma once

#ifndef BROADCAST_H
#define BROADCAST_H

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#include "Structs.h"

DWORD WINAPI broadcastThread(LPVOID lpvParam);

#endif // BROADCAST_H
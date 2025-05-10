#pragma once

#ifndef FILEMAPPING_H
#define FILEMAPPING_H

#include <windows.h>

#define MAP_FILE_NAME TEXT("Local\\TrabSO2")

TCHAR *getMapViewOfFile(DWORD nLetras);

#endif // FILEMAPPING_H
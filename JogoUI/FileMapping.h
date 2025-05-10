#pragma once

#ifndef FILEMAPPING_H
#define FILEMAPPING_H

#include <windows.h>

#define MAXLETRASLIM 12
#define MAP_FILE_NAME TEXT("Local\\TrabSO2")

TCHAR* getMapViewOfFile();

#endif // FILEMAPPING_H
#pragma once

#ifndef REGISTRY_H
#define REGISTRY_H

#include <windows.h>
#include <tchar.h>
#include <fcntl.h>

#define MAXLETRASLIM 12
#define MAXLETRAS 6
#define RITMO 3

HKEY createMainKey();
void getRegistryValues(HKEY hKey, int* maxLetras, int* ritmo);

#endif // REGISTRY_H
#include <tchar.h>
#include <stdio.h>

#include "FileMapping.h"

TCHAR* getMapViewOfFile() {
	HANDLE hFile;
	hFile = OpenFileMapping(
		FILE_MAP_READ,
		FALSE,
		MAP_FILE_NAME
	);

	if (hFile == NULL) {
		_tprintf(TEXT("CreateFileMapping failed (%d).\n"), GetLastError());
		return NULL;
	}

	TCHAR* mappedView = (TCHAR*)MapViewOfFile(
		hFile,
		FILE_MAP_READ,
		0,
		0,
		MAXLETRASLIM * sizeof(TCHAR)
	);

	if (mappedView == NULL) {
		_tprintf(TEXT("MapViewOfFile failed (%d).\n"), GetLastError());
		CloseHandle(hFile);
		return NULL;
	}

	_tprintf(TEXT("MapViewOfFile succeeded.\n"));
	return mappedView;
}
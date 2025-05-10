#include <tchar.h>
#include <stdio.h>

#include "FileMapping.h"

TCHAR *getMapViewOfFile(DWORD nLetras) {
	HANDLE hFile;

	hFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		nLetras * sizeof(TCHAR),
		MAP_FILE_NAME
	);

	if (hFile == NULL) {
		TCHAR Msg[100];

		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			0,
			Msg,
			sizeof(Msg) / sizeof(TCHAR),
			NULL
		);
		_tprintf(TEXT("[GetMapViewOfFile] Mapeamento falhou (%d): %s\n"), GetLastError(), Msg);
		return NULL;
	}

	TCHAR *mappedView = (TCHAR*) MapViewOfFile(
		hFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		nLetras * sizeof(TCHAR)
	);

	if (mappedView == NULL) {
		_tprintf(TEXT("[GetMapViewOfFile] Criação de vista falhou (%d).\n"), GetLastError());
		CloseHandle(hFile);
		return NULL;
	}

	CopyMemory((PVOID)mappedView, TEXT("ABCDEF"), (_tcslen(TEXT("ABCDEF")) * sizeof(TCHAR)));

	_tprintf(TEXT("[GetMapViewOfFile] Vista criada com sucesso.\n"));

	return mappedView;
}
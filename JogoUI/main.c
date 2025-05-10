#include "NamedPipes.h"
#include "FileMapping.h"

#define TAM 100

int _tmain() {
#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
#endif
	HANDLE hPipe;
	HANDLE hBroadcastPipe;
	TCHAR* mappedView = getMapViewOfFile();

	if (mappedView == NULL) {
		_tprintf(TEXT("Erro ao mapear o arquivo.\n"));
		return 1;
	}

	if (!openNamedPipe(&hPipe)) {
		_tprintf(TEXT("Erro ao abrir o Named Pipe.\n"));
		return 1;
	}

	if (!openBroadcastNamedPipe(&hBroadcastPipe)) {
		_tprintf(TEXT("Erro ao abrir o Named Pipe.\n"));
		return 1;
	}

	HANDLE hThread = CreateThread(
		NULL,
		0,
		readServerMessages,
		(LPVOID)hBroadcastPipe,
		0,
		NULL
	);

	if (hThread == NULL)
	{
		_tprintf(TEXT("Erro ao criar thread de conexões: %d\n"), GetLastError());
		return FALSE;
	}

	do
	{

	} while (TRUE);

	CloseHandle(hPipe);
	CloseHandle(hBroadcastPipe);
	UnmapViewOfFile(mappedView);

	return 0;
}
#include "NamedPipes.h"
#include "FileMapping.h"
#include "Structs.h"

#define TAM 100

int _tmain(int argc, TCHAR *argv[]) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	// Verifica se o número de argumentos é válido
	if (argc != 2)
	{
		_tprintf(TEXT("Uso: %s <nome_do_jogador>\n"), argv[0]);
		return 1;
	}

	HANDLE hPipe;
	Broadcast broadcastData;
	BOOL fSuccess = FALSE;
	TCHAR* mappedView = getMapViewOfFile();

	if (mappedView == NULL) {
		_tprintf(TEXT("Erro ao mapear o arquivo.\n"));
		return 1;
	}

	if (!openNamedPipe(&hPipe)) {
		_tprintf(TEXT("Erro ao abrir o Named Pipe.\n"));
		return 1;
	}

	if (!registerOnServer(hPipe, argv[1])) {
		_tprintf(TEXT("Erro ao registar no servidor.\n"));
		CloseHandle(hPipe);
		return 1;
	}

	if (!openBroadcastNamedPipe(&broadcastData.hPipe)) {
		_tprintf(TEXT("Erro ao abrir o Named Pipe.\n"));
		return 1;
	}
	
	broadcastData.running = TRUE;

	HANDLE hThread = CreateThread(
		NULL,
		0,
		readServerMessages,
		(LPVOID)&broadcastData,
		0,
		NULL
	);

	if (hThread == NULL)
	{
		_tprintf(TEXT("Erro ao criar thread de conexões: %d\n"), GetLastError());
		return FALSE;
	}

	TCHAR input[BUFSIZE] = TEXT("");

	do
	{
		TCHAR readBuffer[BUFSIZE] = TEXT("");

		_tprintf(TEXT("> "));

		_fgetts(input, BUFSIZE, stdin);

		input[_tcslen(input) - 1] = TEXT('\0');

		if (input[0] == TEXT(':'))
		{
			if (_tcscmp(input, TEXT(":pont")) != 0 && _tcscmp(input, TEXT(":jogs")) != 0 && _tcscmp(input, TEXT(":sair")) != 0)
			{
				_tprintf(TEXT("Comando inválido.\n"));
				continue;
			}
		}

		fSuccess = WriteFile(
			hPipe,
			input,
			(_tcslen(input) + 1) * sizeof(TCHAR),
			NULL,
			NULL
		);

		if (!fSuccess)
		{
			_tprintf(TEXT("Erro ao enviar o comando: %d\n"), GetLastError());
			CloseHandle(hPipe);
			CloseHandle(broadcastData.hPipe);
			WaitForSingleObject(hThread, INFINITE);
			return 1;
		}

		fSuccess = ReadFile(
			hPipe,
			readBuffer,
			BUFSIZE * sizeof(TCHAR),
			NULL,
			NULL
		);

		if (!fSuccess || GetLastError() == ERROR_BROKEN_PIPE)
		{
			_tprintf(TEXT("Erro ao ler do Named Pipe: %d\n"), GetLastError());
			CloseHandle(hPipe);
			CloseHandle(broadcastData.hPipe);
			WaitForSingleObject(hThread, INFINITE);
			return 1;
		}

		// Mostrar a resposta do servidor
		_tprintf(TEXT("Resposta do servidor: %s\n"), readBuffer);
	} while (_tcscmp(input, TEXT(":sair")) != 0);

	_tprintf(TEXT("A encerrar a thread de broadcast...\n"));

	WaitForSingleObject(hThread, INFINITE);
	
	_tprintf(TEXT("A thread de broadcast foi encerrada.\n"));

	CloseHandle(hPipe);
	UnmapViewOfFile(mappedView);

	return 0;
}
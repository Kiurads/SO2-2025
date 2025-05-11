#include "NamedPipes.h"

BOOL openNamedPipe(HANDLE* hPipe)
{
	_tprintf(TEXT("[OpenNamedPipe] Abrindo Named Pipe...\n"));

	while (1) {
		// Tenta abrir o Named Pipe
		*hPipe = CreateFile(
			PIPE_NAME,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
		);

		if (*hPipe != INVALID_HANDLE_VALUE)
			break;

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_tprintf(TEXT("[OpenNamedPipe] Erro ao abrir Named Pipe: %d\n"), GetLastError());
			return FALSE;
		}

		if (!WaitNamedPipe(PIPE_NAME, 20000))
		{
			_tprintf(TEXT("[OpenNamedPipe] Timeout ao esperar pelo Named Pipe.\n"));
			return FALSE;
		}
	}

	_tprintf(TEXT("[OpenNamedPipe] Named Pipe aberto com sucesso.\n"));

	DWORD dwMode = PIPE_READMODE_MESSAGE;

	if (!SetNamedPipeHandleState(*hPipe, &dwMode, NULL, NULL))
	{
		_tprintf(TEXT("[OpenNamedPipe] Erro ao definir o estado do Named Pipe: %d\n"), GetLastError());
		CloseHandle(*hPipe);
		return FALSE;
	}

	_tprintf(TEXT("[OpenNamedPipe] Estado do Named Pipe definido com sucesso.\n"));

	return TRUE;
}

BOOL openBroadcastNamedPipe(HANDLE* hPipe)
{
	_tprintf(TEXT("[OpenBroadcastNamedPipe] Abrindo Named Pipe...\n"));

	while (1) {
		// Tenta abrir o Named Pipe
		*hPipe = CreateFile(
			BROADCAST_PIPE_NAME,
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
		);

		if (*hPipe != INVALID_HANDLE_VALUE)
			break;

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_tprintf(TEXT("[OpenBroadcastNamedPipe] Erro ao abrir Named Pipe: %d\n"), GetLastError());
			return FALSE;
		}

		if (!WaitNamedPipe(PIPE_NAME, 20000))
		{
			_tprintf(TEXT("[OpenBroadcastNamedPipe] Timeout ao esperar pelo Named Pipe.\n"));
			return FALSE;
		}
	}

	_tprintf(TEXT("[OpenBroadcastNamedPipe] Named Pipe aberto com sucesso.\n"));

	return TRUE;
}

BOOL registerOnServer(HANDLE hPipe, TCHAR* username)
{
	_tprintf(TEXT("[RegisterOnServer] Registando no servidor...\n"));
	BOOL fSuccess = WriteFile(
		hPipe,
		username,
		(_tcslen(username) + 1) * sizeof(TCHAR),
		NULL,
		NULL
	);
	if (!fSuccess)
	{
		_tprintf(TEXT("[RegisterOnServer] Erro ao enviar o nome do jogador: %d\n"), GetLastError());
		CloseHandle(hPipe);
		return FALSE;
	}

	_tprintf(TEXT("[RegisterOnServer] Nome do jogador enviado com sucesso.\n"));

	TCHAR readBuffer[BUFSIZE] = TEXT("");

	fSuccess = ReadFile(
		hPipe,
		readBuffer,
		BUFSIZE * sizeof(TCHAR),
		NULL,
		NULL
	);

	if (!fSuccess || GetLastError() == ERROR_BROKEN_PIPE)
	{
		_tprintf(TEXT("[RegisterOnServer] Erro ao ler do Named Pipe: %d\n"), GetLastError());
		CloseHandle(hPipe);
		return FALSE;
	}

	if (_tcscmp(readBuffer, NO_FREE_PLAYER_ERROR_MESSAGE) == 0)
	{
		_tprintf(TEXT("[RegisterOnServer] Erro: %s\n"), readBuffer);
		CloseHandle(hPipe);
		return FALSE;
	}
	else if (_tcscmp(readBuffer, USERNAME_EXISTS_ERROR_MESSAGE) == 0)
	{
		_tprintf(TEXT("[RegisterOnServer] Erro: %s\n"), readBuffer);
		CloseHandle(hPipe);
		return FALSE;
	}

	_tprintf(TEXT("[RegisterOnServer] Registo no servidor realizado com sucesso.\n"));

	return TRUE;
}

DWORD WINAPI readServerMessages(LPVOID lpvParam)
{
	HANDLE hPipe = (HANDLE)lpvParam;

	TCHAR readBuffer[BUFSIZE] = TEXT("");
	BOOL fSuccess = FALSE;

	_tprintf(TEXT("[ReadServerThread:%d] Aguardando dados do servidor...\n"), GetCurrentThreadId());

	while (TRUE)
	{
		fSuccess = ReadFile(
			hPipe,
			readBuffer,
			BUFSIZE * sizeof(TCHAR),
			NULL,
			NULL
		);

		if (!fSuccess || GetLastError() == ERROR_BROKEN_PIPE)
		{
			_tprintf(TEXT("[ReadServerThread:%d] Erro ao ler do Named Pipe: %d\n"), GetCurrentThreadId(), GetLastError());
			CloseHandle(hPipe);
			return 1;
		}

		_tprintf(TEXT("[ReadServerThread:%d] Dados recebidos: %s\n"), GetCurrentThreadId(), readBuffer);
	}

	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	return 0;
}
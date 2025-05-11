#include "NamedPipes.h"

BOOL createClientNamedPipe(HANDLE* hPipe)
{
	_tprintf(TEXT("[CreateClientNamedPipe] Criando Named Pipe...\n"));

	*hPipe = CreateNamedPipe(
		PIPE_NAME,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		BUFSIZE,
		BUFSIZE,
		0,
		NULL
	);

	if (*hPipe == INVALID_HANDLE_VALUE)
	{
		_tprintf(TEXT("[CreateClientNamedPipe] Erro ao criar Named Pipe: %d\n"), GetLastError());
		return FALSE;
	}

	_tprintf(TEXT("[CreateClientNamedPipe] Named Pipe criado com sucesso.\n"));
	return TRUE;
}

BOOL createBroadcastNamedPipe(HANDLE* hPipe)
{
	_tprintf(TEXT("[CreateBroadcastNamedPipe] Criando Named Pipe...\n"));

	*hPipe = CreateNamedPipe(
		BROADCAST_PIPE_NAME,
		PIPE_ACCESS_OUTBOUND,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		BUFSIZE,
		0,
		0,
		NULL
	);

	if (*hPipe == INVALID_HANDLE_VALUE)
	{
		_tprintf(TEXT("[CreateBroadcastNamedPipe] Erro ao criar Named Pipe: %d\n"), GetLastError());
		return FALSE;
	}

	_tprintf(TEXT("[CreateBroadcastNamedPipe] Named Pipe criado com sucesso.\n"));

	return TRUE;
}

int validatePlayer(ServerData* serverData, TCHAR *username)
{
	_tprintf(TEXT("[ValidatePlayer] Validando jogador...\n"));

	int freePlayerIndex = NO_FREE_PLAYER_ERROR_CODE;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (serverData->players[i].active == FALSE && freePlayerIndex == NO_FREE_PLAYER_ERROR_CODE)
		{
			freePlayerIndex = i;
		}
		else if (serverData->players[i].active == TRUE && _tcscmp(serverData->players[i].name, username) == 0)
		{
			_tprintf(TEXT("[ValidatePlayer] Jogador %d tem o mesmo username.\n"), i);
			return USERNAME_EXISTS_ERROR_CODE;
		}
	}

	if (freePlayerIndex == NO_FREE_PLAYER_ERROR_CODE)
	{
		_tprintf(TEXT("[ValidatePlayer] Não há posições livres.\n"));
	}

	return freePlayerIndex;
}

DWORD WINAPI acceptConnections(LPVOID lpvParam)
{
	ServerData* serverData = (ServerData*)lpvParam;

	HANDLE hThread = NULL;

	_tprintf(TEXT("[AcceptConnections] Aguardando conexões...\n"));

	while (TRUE)
	{
		HANDLE hPipe;
		HANDLE hPipeBroadcast;

		createClientNamedPipe(&hPipe);
		createBroadcastNamedPipe(&hPipeBroadcast);

		if (hPipe == INVALID_HANDLE_VALUE || hPipeBroadcast == INVALID_HANDLE_VALUE)
		{
			_tprintf(TEXT("[AcceptConnections] Erro ao criar Named Pipes: %d\n"), GetLastError());
			return FALSE;
		}

		BOOL fConnected = ConnectNamedPipe(hPipe, NULL) ? 
			TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

		if (fConnected) 
		{
			TCHAR username[BUFSIZE] = TEXT("");
			BOOL fSuccess = FALSE;
			DWORD bytesRead = 0;

			_tprintf(TEXT("[AcceptConnections] Cliente conectado.\n"));
			
			// Lê o nome do jogador do Named Pipe

			fSuccess = ReadFile(
				hPipe,
				username,
				BUFSIZE * sizeof(TCHAR),
				&bytesRead,
				NULL
			);

			if (!fSuccess || GetLastError() == ERROR_BROKEN_PIPE)
			{
				_tprintf(TEXT("[AcceptConnections] Erro ao ler do Named Pipe: %d\n"), GetLastError());
				CloseHandle(hPipe);
				CloseHandle(hPipeBroadcast);
				continue;
			}

			username[bytesRead / sizeof(TCHAR)] = TEXT('\0');

			_tprintf(TEXT("[AcceptConnections] Nome de usuário recebido: %s\n"), username);

			int freePlayerIndex = validatePlayer(serverData, username);

			if (freePlayerIndex < 0)
			{
				if (freePlayerIndex == USERNAME_EXISTS_ERROR_CODE)
				{
					_tprintf(TEXT("[AcceptConnections] Nome de usuário já existe.\n"));

					fSuccess = WriteFile(
						hPipe,
						USERNAME_EXISTS_ERROR_MESSAGE,
						(_tcslen(USERNAME_EXISTS_ERROR_MESSAGE) + 1) * sizeof(TCHAR),
						NULL,
						NULL
					);
				}
				else if (freePlayerIndex == NO_FREE_PLAYER_ERROR_CODE)
				{
					_tprintf(TEXT("[AcceptConnections] Não há posições livres.\n"));

					fSuccess = WriteFile(
						hPipe,
						NO_FREE_PLAYER_ERROR_MESSAGE,
						(_tcslen(NO_FREE_PLAYER_ERROR_MESSAGE) + 1) * sizeof(TCHAR),
						NULL,
						NULL
					);
				}

				if (!fSuccess)
				{
					_tprintf(TEXT("[AcceptConnections] Erro ao enviar mensagem de erro: %d\n"), GetLastError());
					CloseHandle(hPipe);
					CloseHandle(hPipeBroadcast);
					continue;
				}

				CloseHandle(hPipe);
				CloseHandle(hPipeBroadcast);
				continue;
			}

			// Envia mensagem de sucesso para o cliente
			fSuccess = WriteFile(
				hPipe,
				SUCCESSFUL_CONNECTION_MESSAGE,
				(_tcslen(SUCCESSFUL_CONNECTION_MESSAGE) + 1) * sizeof(TCHAR),
				NULL,
				NULL
			);

			if (!fSuccess)
			{
				_tprintf(TEXT("[AcceptConnections] Erro ao enviar mensagem de sucesso: %d\n"), GetLastError());
				CloseHandle(hPipe);
				CloseHandle(hPipeBroadcast);
				continue;
			}

			fConnected = ConnectNamedPipe(hPipeBroadcast, NULL) ?
				TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

			if (!fConnected)
			{
				_tprintf(TEXT("[AcceptConnections] Erro ao conectar o Named Pipe de Broadcast: %d\n"), GetLastError());
				CloseHandle(hPipe);	
				CloseHandle(hPipeBroadcast);
				continue;
			}

			// Envia mensagem de sucesso de broadcast
			_stprintf_s(serverData->broadcastData->message, BROADCAST_MESSAGE_SIZE, TEXT("Jogador %s entrou no jogo"), username);

			SetEvent(serverData->broadcastData->hEvent);

			// Preenche os dados do jogador
			serverData->players[freePlayerIndex].active = TRUE;
			_tcscpy(serverData->players[freePlayerIndex].name, username);
			serverData->players[freePlayerIndex].hPipe = hPipe;
			serverData->players[freePlayerIndex].hPipeBroadcast = hPipeBroadcast;
			serverData->players[freePlayerIndex].broadcastData = serverData->broadcastData;

			hThread = CreateThread(
				NULL,
				0,
				manageClientThread,
				(LPVOID) &serverData->players[freePlayerIndex],
				0,
				NULL
			);

			if (hThread == NULL)
			{
				_tprintf(TEXT("[AcceptConnections] Erro ao criar thread: %d\n"), GetLastError());
				return FALSE;
			}

			serverData->players[freePlayerIndex].hThread = hThread;

			_tprintf(TEXT("[AcceptConnections] Thread criada com sucesso.\n"));
		}
		else
			CloseHandle(hPipe);
	}

	return TRUE;
}

DWORD WINAPI manageClientThread(LPVOID lpvParam)
{
	Player* player = (Player*)lpvParam;

	TCHAR readBuffer[BUFSIZE] = TEXT("");
	BOOL fSuccess = FALSE;

	_tprintf(TEXT("[ManageClientThread:%d] Aguardando dados do cliente...\n"), GetCurrentThreadId());

	while (TRUE)
	{
		fSuccess = ReadFile(
			player->hPipe,
			readBuffer,
			BUFSIZE * sizeof(TCHAR),
			NULL,
			NULL
		);

		if (!fSuccess || GetLastError() == ERROR_BROKEN_PIPE)
		{
			_tprintf(TEXT("[ManageClientThread:%d] Erro ao ler do Named Pipe: %d\n"), GetCurrentThreadId(), GetLastError());
			CloseHandle(player->hPipe);
			return 1;
		}

		_tprintf(TEXT("[ManageClientThread:%d] Dados recebidos: %s\n"), GetCurrentThreadId(), readBuffer);
	}

	DisconnectNamedPipe(player->hPipe);
	CloseHandle(player->hPipe);

	return 0;
}

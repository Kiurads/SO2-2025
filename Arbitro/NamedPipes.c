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
			_tprintf(TEXT("[AcceptConnections] Cliente conectado.\n"));

			fConnected = ConnectNamedPipe(hPipeBroadcast, NULL) ?
				TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

			if (!fConnected)
			{
				_tprintf(TEXT("[AcceptConnections] Erro ao conectar o Named Pipe de Broadcast: %d\n"), GetLastError());
				CloseHandle(hPipe);
				continue;
			}

			// Adiciona o novo jogador à lista de jogadores
			int i;

			for (i = 0; i < MAX_PLAYERS; i++)
			{
				if (serverData->players[i].active == FALSE)
				{
					serverData->players[i].active = TRUE;
					_tprintf(TEXT("[AcceptConnections] Jogador %d conectado.\n"), i);
					break;
				}
			}

			if (i >= MAX_PLAYERS)
			{
				_tprintf(TEXT("[AcceptConnections] Limite de jogadores atingido.\n"));
				CloseHandle(hPipe);
				CloseHandle(hPipeBroadcast);
				continue;
			}

			serverData->players[i].hPipe = hPipe;
			serverData->players[i].hPipeBroadcast = hPipeBroadcast;

			serverData->players[i].broadcastData = serverData->broadcastData;

			hThread = CreateThread(
				NULL,
				0,
				manageClientThread,
				(LPVOID) &serverData->players[i],
				0,
				NULL
			);

			if (hThread == NULL)
			{
				_tprintf(TEXT("[AcceptConnections] Erro ao criar thread: %d\n"), GetLastError());
				return FALSE;
			}

			serverData->players[i].hThread = hThread;

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

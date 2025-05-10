#include "Broadcast.h"

DWORD WINAPI broadcastThread(LPVOID lpvParam)
{
	ServerData* serverData = (ServerData*)lpvParam;

	_tprintf(TEXT("[BroadcastThread] Iniciando thread de broadcast...\n"));

	while (1)
	{
		WaitForSingleObject(serverData->broadcastData->hEvent, INFINITE);

		// Broadcast the message to all players
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			if (serverData->players[i].active && serverData->players[i].hPipeBroadcast != NULL)
			{
				DWORD bytesWritten;
				BOOL result = WriteFile(
					serverData->players[i].hPipeBroadcast,
					serverData->broadcastData->message,
					BROADCAST_MESSAGE_SIZE,
					&bytesWritten,
					NULL
				);
				if (!result)
				{
					_tprintf(TEXT("[BroadcastThread] Erro ao enviar mensagem para o jogador %d: %d\n"), i, GetLastError());
				}
			}
		}
	}
}
#include <stdio.h>
#include <tchar.h>

#include "Structs.h"
#include "Registry.h"
#include "FileMapping.h"
#include "Broadcast.h"
#include "NamedPipes.h"
#include "Events.h"

int _tmain() {
#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
#endif

	ServerData serverData;
	BroadcastData broadcastData;

    serverData.hKey = NULL;
	serverData.maxletras = 0;
	serverData.ritmo = 0;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		serverData.players[i].active = FALSE;
	}

	serverData.hKey = createMainKey();

    if (serverData.hKey == NULL)
    {
        return 1;
    }

    getRegistryValues(serverData.hKey, &serverData.maxletras, &serverData.ritmo);

	TCHAR* mappedView = getMapViewOfFile(serverData.maxletras);

	if (mappedView == NULL)
	{
		_tprintf(TEXT("Erro ao mapear o arquivo.\n"));
		return 1;
	}

	broadcastData.hEvent = createBroadcastEvent();

	if (broadcastData.hEvent == NULL)
	{
		_tprintf(TEXT("Erro ao criar evento de Broadcast\n"));
		return 1;
	}

	serverData.broadcastData = &broadcastData;

	HANDLE hThreadConnections = CreateThread(
		NULL,
		0,
		acceptConnections,
		(LPVOID)&serverData,
		0,
		NULL
	);

	if (hThreadConnections == NULL)
	{
		_tprintf(TEXT("Erro ao criar thread de conexões: %d\n"), GetLastError());
		return FALSE;
	}

	HANDLE hThreadBroadcast = CreateThread(
		NULL,
		0,
		broadcastThread,
		(LPVOID)&serverData,
		0,
		NULL
	);

	if (hThreadBroadcast == NULL)
	{
		_tprintf(TEXT("Erro ao criar thread de conexões: %d\n"), GetLastError());
		return FALSE;
	}

	TCHAR command[BUFSIZE] = TEXT("");

	do
	{
		_tprintf(TEXT("Digite o comando: "));
		_fgetts(command, BUFSIZE, stdin);

		command[_tcslen(command) - 1] = '\0';

		// Obter a primeira palavra do comando
		TCHAR* firstWord = _tcstok(command, TEXT(" "));

		_tprintf(TEXT("Comando recebido: %s\n"), firstWord);

		if (_tcscmp(firstWord, TEXT("listar")) == 0)
		{
			_tprintf(TEXT("Comando listar recebido.\n"));
		}
		else if (_tcscmp(firstWord, TEXT("excluir")) == 0)
		{
			_tprintf(TEXT("Comando excluir recebido.\n"));
		}
		else if (_tcscmp(firstWord, TEXT("iniciarbot")) == 0)
		{
			_tprintf(TEXT("Comando iniciarbot recebido.\n"));
		}
		else if (_tcscmp(firstWord, TEXT("acelerar")) == 0)
		{
			_tprintf(TEXT("Comando acelerar recebido.\n"));
		}
		else if (_tcscmp(firstWord, TEXT("travar")) == 0)
		{
			_tprintf(TEXT("Comando travar recebido.\n"));
		}
		else if (_tcscmp(firstWord, TEXT("encerrar")) == 0)
		{
			_tprintf(TEXT("Comando encerrar recebido.\n"));
		}
		else if (_tcscmp(firstWord, TEXT("broadcast")) == 0)
		{
			// Enviar mensagem de broadcast
			_tcscpy(serverData.broadcastData->message, _tcsdup(command + _tcslen(firstWord) + 1));

			_tprintf(TEXT("Mensagem de broadcast: %s\n"), serverData.broadcastData->message);

			SetEvent(serverData.broadcastData->hEvent);
		}
		else
		{
			_tprintf(TEXT("Comando inválido.\n"));
		}
	} while (_tcscmp(command, TEXT("encerrar")) != 0);
}
#include "Events.h"

HANDLE createBroadcastEvent()
{
	HANDLE hEvent = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		NULL
	);

	if (hEvent == NULL)
	{
		_tprintf(TEXT("[CreateBroadcastEvent] Erro ao criar evento de broadcast: %d\n"), GetLastError());
		return NULL;
	}

	_tprintf(TEXT("[CreateBroadcastEvent] Evento de broadcast criado com sucesso.\n"));

	return hEvent;
}
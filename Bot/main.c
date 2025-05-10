#include <stdio.h>
#include <tchar.h>
#include <fcntl.h>

#define TAM 100

int _tmain() {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	return 0;
}
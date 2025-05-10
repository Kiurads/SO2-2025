#include "Registry.h"

HKEY createMainKey() {
	TCHAR* regPath = TEXT("Software\\TrabSO2");

	HKEY hKey = NULL;
	DWORD keyStatus;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, regPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &keyStatus) == ERROR_SUCCESS) {
		if (keyStatus == REG_CREATED_NEW_KEY) {
			_tprintf(TEXT("Chave criada com sucesso em %s!\n"), regPath);
		}
		else {
			_tprintf(TEXT("Chave aberta com sucesso em %s!\n"), regPath);
		}
	}
	else {
		_tprintf(TEXT("Erro ao criar ou abrir a chave em %s!\n"), regPath);
	}

	return hKey;
}

void getRegistryValues(HKEY hKey, int* maxLetras, int* ritmo) {
	DWORD maxLetrasSize = sizeof(int);
	DWORD ritmoSize = sizeof(int);

	_tprintf(TEXT("Lendo valores do registro...\n"));

	if (RegGetValue(hKey, TEXT("MAXLETRAS"), NULL, RRF_RT_DWORD, NULL, maxLetras, &maxLetrasSize) != ERROR_SUCCESS)
	{
		_tprintf(TEXT("MAXLETRAS criado com o valor padrão: %d\n"), MAXLETRAS);

		HKEY maxLetrasKey;

		*maxLetras = MAXLETRAS;

		RegCreateKey(HKEY_CURRENT_USER, TEXT("Software\\TrabSO2\\MAXLETRAS"), &maxLetrasKey);

		RegSetValueEx(maxLetrasKey, NULL, 0, REG_DWORD, (const BYTE*)maxLetras, sizeof(int));
	}
	else
	{
		_tprintf(TEXT("MAXLETRAS lido com sucesso: %d\n"), *maxLetras);

		if (*maxLetras > MAXLETRASLIM) {
			*maxLetras = MAXLETRASLIM;

			HKEY maxLetrasKey;

			RegCreateKey(HKEY_CURRENT_USER, TEXT("Software\\TrabSO2\\MAXLETRAS"), &maxLetrasKey);

			RegSetValueEx(maxLetrasKey, NULL, 0, REG_DWORD, (const BYTE*)maxLetras, sizeof(int));

			_tprintf(TEXT("MAXLETRAS maior que 12, alterando para 12!\n"));
		}
	}

	if (RegGetValue(hKey, TEXT("RITMO"), NULL, RRF_RT_DWORD, NULL, ritmo, &ritmoSize) != ERROR_SUCCESS)
	{
		HKEY ritmoKey;

		*ritmo = RITMO;

		RegCreateKey(HKEY_CURRENT_USER, TEXT("Software\\TrabSO2\\RITMO"), &ritmoKey);

		RegSetValueEx(ritmoKey, NULL, 0, REG_DWORD, (const BYTE*)ritmo, sizeof(int));

		_tprintf(_T("RITMO criado com o valor padrão: %d\n"), RITMO);
	}
	else
	{
		_tprintf(_T("RITMO lido com sucesso: %d\n"), *ritmo);
	}
}
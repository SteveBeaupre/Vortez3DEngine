#ifndef _CREGLIB_H_
#define _CREGLIB_H_
#ifdef __cplusplus

#include "Windows.h"
#include "Stdio.h"

class CRegLib {
public:
	CRegLib();
	~CRegLib();
private:
	HKEY phKey;
public:

	HKEY GetHKey();
	BOOL CreateKey(HKEY hRootKey, LPCTSTR lpszSubKey, PHKEY phKey);
	BOOL OpenKey(HKEY hRootKey, LPCTSTR lpszSubKey, PHKEY phKey);
	void CloseKey();

	BOOL QueryValue(LPCTSTR lpValueName, LPDWORD lpdwType, LPBYTE lpbData, LPDWORD lpcbData);
	BOOL SetValue(LPCTSTR lpValueName, DWORD dwType, CONST BYTE* lpData, DWORD cbData);
	BOOL DeleteValue(LPTSTR lpszValue);

	BOOL DeleteKey(HKEY hRootKey, LPCTSTR lpszSubKey);
};


#endif
#endif //--_CREGLIB_H_

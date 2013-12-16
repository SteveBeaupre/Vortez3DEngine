#include "RegLib.h"

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

CRegLib::CRegLib()
{
	phKey = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////

CRegLib::~CRegLib()
{
	this->CloseKey();
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

HKEY CRegLib::GetHKey()
{
	return phKey;
}

/////////////////////////////////////////////////////////////////////////////////////////////

BOOL CRegLib::CreateKey(HKEY hRootKey, LPCTSTR lpszSubKey, PHKEY pKey)
{
	LPDWORD  lpdwDisposition = NULL;

	DWORD Res = RegCreateKeyEx(
		hRootKey,                // handle of an open key 
		lpszSubKey,	             // address of subkey name 
		0L,	                     // reserved 
		NULL,	                 // address of class string 
		REG_OPTION_NON_VOLATILE, // special options flag 
		KEY_ALL_ACCESS,	         // desired security access 
		NULL,	                 // address of key security structure 
		pKey,	                 // address of buffer for opened handle  
		lpdwDisposition 	     // address of disposition value buffer 
	) == ERROR_SUCCESS;
	phKey = *pKey;

	return Res;
}

/////////////////////////////////////////////////////////////////////////////////////////////

BOOL CRegLib::OpenKey(HKEY hRootKey, LPCTSTR lpszSubKey, PHKEY pKey)
{
	DWORD Res = RegOpenKeyEx(
		hRootKey,       // handle of open key 
		lpszSubKey,	    // address of name of subkey to open 
		0L,	            // reserved 
		KEY_ALL_ACCESS,	// security access mask 
		pKey 	        // address of buffer for opened handle  
	) == ERROR_SUCCESS;
	phKey = *pKey;

	return Res;
}

/////////////////////////////////////////////////////////////////////////////////////////////

BOOL CRegLib::DeleteKey(HKEY hRootKey, LPCTSTR lpszSubKey)
{
	return RegDeleteKey(
		hRootKey,  // handle of an open key 
		lpszSubKey // address of subkey name 
	) == ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

BOOL CRegLib::QueryValue(LPCTSTR lpValueName, LPDWORD lpdwType, LPBYTE lpbData, LPDWORD lpcbData)
{
	if(phKey){
		return RegQueryValueEx(
			phKey,	        // handle of open key 
			lpValueName,    // address of name of subkey to open 
			0L,	            // reserved 
			lpdwType,       // address of buffer for value type 
			lpbData,        // address of data buffer 
			lpcbData        // address of data buffer size 
		) == ERROR_SUCCESS;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

BOOL CRegLib::SetValue(LPCTSTR lpValueName, DWORD dwType, CONST BYTE* lpData, DWORD cbData)
{
	if(phKey){
		return RegSetValueEx(
			phKey,	        // handle of open key 
			lpValueName,    // address of name of subkey to open 
			0L,	            // reserved 
			dwType,         // flag for value type 
			lpData,         // address of value data 
			cbData          // size of value data 
		) == ERROR_SUCCESS;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

BOOL CRegLib::DeleteValue(LPTSTR lpszValue)
{
	if(phKey){
		return RegDeleteValue(
			phKey,	        // handle of key 
			lpszValue 	    // address of value name 
		) == ERROR_SUCCESS;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void CRegLib::CloseKey()
{
	if(phKey){
		RegCloseKey(
			phKey            // handle of key to close 
		);
		phKey = NULL;
	}
}


#ifndef _CREGISTRY_H_
#define _CREGISTRY_H_
#ifdef __cplusplus

#include "Windows.h"
#include "Stdio.h"

class CRegistry {
private:
	bool OpenKey(HKEY MainKey, LPCTSTR SubKey, PHKEY phKey);
	void CloseKey(HKEY hKey);
public:
	void CreateKey(HKEY MainKey, LPCTSTR SubKey);
	void DeleteKey(HKEY MainKey, LPCTSTR SubKey);
	bool DoesKeyExist(HKEY MainKey, LPCTSTR SubKey);
	
	bool GetValue(HKEY MainKey, LPCTSTR SubKey, LPCTSTR Value, DWORD Type, BYTE *pBuffer, DWORD *pBufferSize);
	UINT GetValueType(HKEY MainKey, LPCTSTR SubKey, LPCTSTR Value);
	void SetValue(HKEY MainKey, LPCTSTR SubKey, LPCTSTR Value, DWORD Type, BYTE *pBuffer, DWORD BufferSize);
	void DeleteValue(HKEY MainKey, LPCTSTR SubKey, LPCTSTR Value);
	bool DoesValueExist(HKEY MainKey, LPCTSTR SubKey, LPCTSTR Value);
};


#endif
#endif //--_CREGISTRY_H_

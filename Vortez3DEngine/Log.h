#ifndef _CLOG_H_
#define _CLOG_H_
#ifdef __cplusplus
//----------------------------------------------------------------------//
#define EXP_FUNC __declspec(dllexport)
//----------------------------------------------------------------------//
#include "Windows.h"
#include "stdio.h"
//----------------------------------------------------------------------//

class EXP_FUNC CLog {
public:
	CLog();
	~CLog();
private:
	FILE *LogF;
public:
	bool Enabled;

	void Create(char *fname);
	void Close();

	void Write(const char *fmt, ...);
};

#endif
#endif //--_CLOG_H_

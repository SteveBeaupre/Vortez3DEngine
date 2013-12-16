#ifndef _CSTRINGUTILS_H_
#define _CSTRINGUTILS_H_
#ifdef __cplusplus

#include <Windows.h>
#include "Strings.h"
#include <direct.h>

#include "Preproc.h"

#ifdef COMPILE_FOR_BORLAND
// Create an abstract class of the object to export
class AStringsUtils {
public:
	//virtual void _INTERNAL_LINKAGE Free() = 0;
};
#endif

#ifdef COMPILE_FOR_BORLAND
class CStringsUtils  : public AStringsUtils {
#else
class _EXP_FUNC CStringsUtils {
#endif
public:
	CStringsUtils();
	~CStringsUtils();
private:
public:
	
	Strings BoolToStr(bool b);
	bool StrToBool(Strings s);

	Strings LowerCase(Strings s);
	Strings UpperCase(Strings s);
	
	Strings QuotedStr(Strings s);

	/*Trim

	IsPathDelimiter
	LastDelimiter
	ExcludeTrailingPathDelimiter
	IncludeTrailingPathDelimiter*/

	Strings GetCurrentDir();
	void SetCurrentDir(Strings Dir);

	Strings ExpandFileName(Strings s);
	Strings ExtractFileDir(Strings s);
	Strings ExtractFileDrive(Strings s);
	Strings ExtractFileExt(Strings s);
	Strings ExtractFileName(Strings s); 
	Strings ExtractFilePath(Strings s); 
	Strings ExtractShortPathName(Strings s);

	/*Now
	Date
	DateTimeToStr

	CurrentYear
	DayOfWeek
	IsLeapYear*/

#ifdef COMPILE_FOR_BORLAND
	void _INTERNAL_LINKAGE Free(){if(this) delete this;}
#endif
};

#endif
#endif //_CSTRINGUTILS_H_/*
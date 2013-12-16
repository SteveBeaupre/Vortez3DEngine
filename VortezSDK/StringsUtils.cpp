#include "StringsUtils.h"


Strings CStringsUtils::BoolToStr(bool b)
{
	Strings s;
	switch(b)
	{
	case false: s = "False";
	case true:  s = "True";
	}

	return s;
}

bool CStringsUtils::StrToBool(Strings s)
{
	bool b = false;
	if(this->UpperCase(s) == "TRUE")
		b = true;
	return b;
}

Strings CStringsUtils::LowerCase(Strings s)
{
	char *pStr = s.c_str();
	int Len = strlen(pStr);
	for(int Cpt = 0; Cpt < Len; Cpt++){
		if(pStr[Cpt] >= 65 && pStr[Cpt] <= 90)
			pStr[Cpt] += 32;
	}
	return s;
}

Strings CStringsUtils::UpperCase(Strings s)
{
	char *pStr = s.c_str();
	int Len = strlen(s.c_str());
	for(int Cpt = 0; Cpt < Len; Cpt++){
		if(pStr[Cpt] >= 97 && pStr[Cpt] <= 122)
			pStr[Cpt] -= 32;
	}
	return s;
}

Strings CStringsUtils::QuotedStr(Strings s)
{
	Strings Res;
	Res = """";
	Res += s;
	Res += """";
	return Res;
}

Strings CStringsUtils::GetCurrentDir()
{
	char Buf[MAX_PATH];
	ZeroMemory(&Buf[0], MAX_PATH);
	_getcwd(&Buf[0], MAX_PATH);

	Strings s(&Buf[0]);
	return s;
}

void CStringsUtils::SetCurrentDir(Strings Dir)
{
	#ifndef RAD_STUDIO_XE
	_chdir(Dir.c_str());
	#else
	chdir(Dir.c_str());
	#endif
}

/*Strings CStringsUtils::ExpandFileName(Strings s)
{
	return GetCurrentDir() + "\\" + s;
}

Strings CStringsUtils::ExtractFileDir(Strings s)
{
	Strings Res = "";
	int Len = s.strsize();

	int Cpt = Len - 1;
	while(Len >= 0){
	
	
		Cpt--;
	}
}

Strings CStringsUtils::ExtractFileDrive(Strings s)
{

}

Strings CStringsUtils::ExtractFileExt(Strings s)
{

}

Strings CStringsUtils::ExtractFileName(Strings s)
{

}
 
Strings CStringsUtils::ExtractFilePath(Strings s)
{

}
 
Strings CStringsUtils::ExtractShortPathName(Strings s)
{

}*/


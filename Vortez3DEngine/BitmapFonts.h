#ifndef _CBITMAPFONTS_H_
#define _CBITMAPFONTS_H_
#ifdef __cplusplus
//----------------------------------------------------------------------//
#ifdef COMPILE_FOR_DIRECTX9
#else 
#define COMPILE_FOR_OPENGL
#endif
#ifdef COMPILE_FOR_OPENGL
//----------------------------------------------------------------------//
#define EXP_FUNC __declspec(dllexport)
//----------------------------------------------------------------------//
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
//----------------------------------------------------------------------//
#include <gl\gl.h>
#include <gl\glu.h>
//----------------------------------------------------------------------//

class EXP_FUNC CBitmapFonts
{
public:
	CBitmapFonts();
	~CBitmapFonts();
public:   
	struct CBitmapFontSettings {
		HDC hDC;
		char *FontName;
		int FontSize;
		int  l,t;			
		BYTE r,g,b;
	};
private:
	bool Builded;
	int  base;
	int  Height;
private:
	CBitmapFontSettings Settings;
public:   
	void Build(CBitmapFontSettings *pSettings);
	void Build(HDC hDC, char *FontName, int FontSize, int Left, int Top, BYTE r, BYTE g, BYTE b);
    void Free();
public:   
	void glPrintArg(int l, int t, const char *fmt, ...);
	void glPrintTxt(int l, int t, const char *text);
	void glPrintArg(const char *fmt, ...);
	void glPrintTxt(const char *text);
public:   
	void SetPosition(int L, int T);
	void IncPosition(int L, int T);
	void SetColor(BYTE r, BYTE g, BYTE b);
	void SetSettings(CBitmapFontSettings *pSettings);
};
#endif

#endif
#endif //--_CBITMAPFONTS_H_

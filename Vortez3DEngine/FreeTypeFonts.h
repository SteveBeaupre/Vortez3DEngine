#ifndef _CFREETYPEFONT_H_
#define _CFREETYPEFONT_H_
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
#ifndef _CPPBUILDER6_
#pragma comment(lib, "libfreetype.lib")
#else
#pragma comment(lib, "libfreetype-bcc.lib")
#endif
//----------------------------------------------------------------------//
//FreeType Headers
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
//----------------------------------------------------------------------//
//OpenGL Headers 
#include <windows.h>		//(the GL headers need it)
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
//----------------------------------------------------------------------//
#include "SafeKill.h"
//----------------------------------------------------------------------//

//#define PI 3.14159265f

#define NO_FONTS_BLENDING     0
#define BLEND_FONT_NORMAL     1
#define BLEND_FONT_SPECIAL    2

#define FONT_FILTER_LINEAR   0
#define FONT_FILTER_NEAREST  1

class EXP_FUNC CFreeTypeFonts {
public:
	CFreeTypeFonts();
	~CFreeTypeFonts();
public:
	struct CFreeTypeFontSettings {
		int   L,T;
		float SX,SY;
		BYTE  R,G,B;
	};
private:
	bool Builded;

	GLuint *TexID;
	GLuint ListBase;

	CFreeTypeFontSettings Settings;

	float FontHeight;
	UINT  FontWidth[256];
	UINT  MaxWidthPerLines;
	BOOL  BlendingMode;
	BOOL  FilterTexture;
	
	int  NextPowerOf2(int a);
	void PushScreenCoordinateMatrix();
	void PopProjectionMatrix();

	bool MakeDisplayList(FT_Face face, BYTE ch, GLuint list_base, GLuint* tex_base);
	
	UINT ParseTextToLine(BYTE *pText, UINT MaxWidth);

public:
	void SetPosition(int l, int t);
	void SetScale(float sx, float sy);
	void SetColor(BYTE r, BYTE g, BYTE b);
	void SetSettings(CFreeTypeFontSettings *pSettings);

	void CreateBitmapPreview(char *FName, BYTE* pFontDataBuf, UINT FontBufSize, UINT h);
	
	bool Load(char* fname, UINT h, BYTE bBlendingMode = BLEND_FONT_NORMAL, BYTE FilteringMode = FONT_FILTER_LINEAR);
	bool LoadFromFile(char* fname, UINT h, BYTE bBlendingMode = BLEND_FONT_NORMAL, BYTE FilteringMode = FONT_FILTER_LINEAR);
	bool LoadFromMemory(BYTE* pFontDataBuf, UINT FontBufSize, UINT h, BYTE bBlendingMode = BLEND_FONT_NORMAL, BYTE FilteringMode = FONT_FILTER_LINEAR);
	void Free();
	
	UINT CalcFirstVisibleCharsNum(char *pText, UINT MaxWidth);
	UINT CalcNumCharsOverLimit(char *pText, UINT MaxWidth);
	UINT CalcNumLines(char *pText, UINT MaxWidth);
	UINT CalcCursorOffset(BYTE *pText);

	//void PrintTxt(float x, float y, float sx, float sy, float r, float g, float b, const char *text);
	void PrintArg(UINT MaxWidth, UINT BufferSize, bool Rescale, const char *fmt, ...);
	void PrintArg(UINT MaxWidth, UINT BufferSize, bool Rescale, float x, float y, float sx, float sy, float r, float g, float b, const char *fmt, ...);
	void Print(UINT MaxWidth, UINT BufferSize, bool Rescale, float x, float y, float sx, float sy, float r, float g, float b, const char *text);
};
#endif

#endif
#endif //_CFREETYPEFONT_H_

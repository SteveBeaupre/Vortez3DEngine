#include "BitmapFonts.h"

#ifdef COMPILE_FOR_OPENGL

const int MaxFont = 96;

CBitmapFonts::CBitmapFonts()
{
	Builded = false;
}

CBitmapFonts::~CBitmapFonts()
{
	Free();
}

void CBitmapFonts::Build(HDC hDC, char *FontName, int FontSize, int Left, int Top, BYTE r, BYTE g, BYTE b)
{
	CBitmapFontSettings bfs;
	bfs.hDC = hDC;
	bfs.FontName = FontName;
	bfs.FontSize = FontSize;
	bfs.l = Left;
	bfs.t = Top;
	bfs.r = r;
	bfs.g = g;
	bfs.b = b;
	Build(&bfs);
}

void CBitmapFonts::Build(CBitmapFontSettings *pSettings)
{
	if(Builded){return;}
	SetSettings(pSettings);
	Height = pSettings->FontSize;

	// Generate storage for 96 chars
	base = glGenLists(MaxFont);	

	// Create/Load the font
	HFONT  font = CreateFont(-Height,     // Height Of Font
					  0,			      // Width Of Font
					  0,			      // Angle Of Escapement
					  0,			      // Orientation Angle
					  FW_THIN/*FW_BOLD*/, // Font Weight
					  FALSE,	          // Italic
					  FALSE,	          // Underline
					  FALSE,	          // Strikeout
					  ANSI_CHARSET,       // Character Set Identifier
					  OUT_TT_PRECIS,      // Output Precision
					  CLIP_DEFAULT_PRECIS,// Clipping Precision
					  ANTIALIASED_QUALITY,// Output Quality
					  FF_DONTCARE|DEFAULT_PITCH,// Family And Pitch
					  pSettings->FontName);     // Font Name

	// Selects The Font We Want
	SelectObject(pSettings->hDC, font);
	// Builds 96 characters starting at character 32
	wglUseFontBitmaps(pSettings->hDC, 32, MaxFont, base);

	Builded = true;
}

void CBitmapFonts::SetPosition(int l, int t)
{
	Settings.l = l; 
	Settings.t = t;
}

void CBitmapFonts::IncPosition(int l, int t)
{
	Settings.l += l; 
	Settings.t += t;
}

void CBitmapFonts::SetColor(BYTE r, BYTE g, BYTE b)
{
	Settings.r = r;
	Settings.g = g;
	Settings.b = b;
}

void CBitmapFonts::SetSettings(CBitmapFontSettings *pSettings)
{
	memcpy(&Settings, pSettings, sizeof(CBitmapFontSettings));
}

void CBitmapFonts::Free()
{
	if(Builded){
		glDeleteLists(base, MaxFont);
		Builded = false;
	}
}

void CBitmapFonts::glPrintArg(int l, int t, const char *fmt, ...)
{
	SetPosition(l,t);
	if(!Builded || fmt == NULL)
		return;

	va_list ap;
	char text[1024];

	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);

	glPrintTxt(&text[0]);
}

void CBitmapFonts::glPrintTxt(int l, int t, const char *text)
{
	SetPosition(l,t);
	glPrintTxt(text);
}

void CBitmapFonts::glPrintArg(const char *fmt, ...)
{
	if(!Builded || fmt == NULL)
		return;

	va_list ap;
	char text[1024];

	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);

	glPrintTxt(&text[0]);
}

void CBitmapFonts::glPrintTxt(const char *text)
{
	if(!Builded){return;}
	glDisable(GL_TEXTURE_2D);

	// Set the text color
	glColor3ub(Settings.r, Settings.g, Settings.b);
	// Position The Text On The Screen
	glRasterPos2i(Settings.l, Settings.t + Height);

	glPushAttrib(GL_LIST_BIT); // Pushes The Display List Bits
	glListBase(base - 32);	   // Sets The Base Character to 32
	
	if(text != NULL)           // Draws The Display List Text
		glCallLists((int)strlen(text), GL_UNSIGNED_BYTE, text);	

	glPopAttrib();			   // Pops The Display List Bits
}

#endif
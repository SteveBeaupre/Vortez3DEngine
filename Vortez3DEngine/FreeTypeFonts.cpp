#include "FreeTypeFonts.h"

#ifdef COMPILE_FOR_OPENGL

const int MaxTextures = 256;

CFreeTypeFonts::CFreeTypeFonts()
{
	Builded = false;
}

CFreeTypeFonts::~CFreeTypeFonts()
{
	Free();
}

///This function gets the first power of 2 >= the int that we pass it
int CFreeTypeFonts::NextPowerOf2(int a)
{
	int rval = 2;

	while(rval < a)
		rval <<= 1;

	return rval;
}

void CFreeTypeFonts::SetPosition(int l, int t)
{
	Settings.L = l;
	Settings.T = t;
}

void CFreeTypeFonts::SetScale(float sx, float sy)
{
	Settings.SX = sx;
	Settings.SY = sy;
}

void CFreeTypeFonts::SetColor(BYTE r, BYTE g, BYTE b)
{
	Settings.R = r;
	Settings.G = g;
	Settings.B = b;
}

void CFreeTypeFonts::SetSettings(CFreeTypeFontSettings *pSettings)
{
	memcpy(&Settings, pSettings, sizeof(CFreeTypeFontSettings));
}

/// A fairly straight forward function that pushes
/// a projection matrix that will make object world 
/// coordinates identical to window coordinates.
void CFreeTypeFonts::PushScreenCoordinateMatrix() 
{
	glPushAttrib(GL_TRANSFORM_BIT);
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//gluOrtho2D(vp[0],vp[2],vp[1],vp[3]);
	gluOrtho2D(vp[0],vp[2],vp[3],vp[1]);
	glPopAttrib();
}

/// Pops the projection matrix without changing the current MatrixMode
void CFreeTypeFonts::PopProjectionMatrix() 
{
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}


bool CFreeTypeFonts::Load(char* fname, UINT h, BYTE bBlendingMode, BYTE FilteringMode)
{
	// Get the windows directory
	char WinDir[256];
	ZeroMemory(&WinDir[0], 256);
	GetWindowsDirectory(&WinDir[0], 256);

	// Create the file name
	char FontFileName[256];
	ZeroMemory(&FontFileName[0], 256);
	sprintf(&FontFileName[0], "%s\\fonts\\%s.TTF", &WinDir[0], fname);

	return LoadFromFile(&FontFileName[0], h, bBlendingMode);
}

bool CFreeTypeFonts::LoadFromFile(char* fname, UINT h, BYTE bBlendingMode, BYTE FilteringMode) 
{
	BlendingMode = bBlendingMode;
	FilterTexture = FilteringMode == FONT_FILTER_LINEAR;

	//Allocate some memory to store the texture ids.
	TexID = new GLuint[MaxTextures];

	this->FontHeight = (float)h;

	//Create and initilize a freetype font library.
	FT_Library library;
	if(FT_Init_FreeType(&library)) 
		return false;

	//The object in which Freetype holds information on a given
	//font is called a "face".
	FT_Face face;

	//This is where we load in the font information from the file.
	//Of all the places where the code might die, this is the most likely,
	//as FT_New_Face will die if the font file does not exist or is somehow broken.
	if(FT_New_Face(library, fname, 0, &face))
		return false;

	//For some twisted reason, Freetype measures font size
	//in terms of 1/64ths of pixels.  Thus, to make a font
	//h pixels high, we need to request a size of h*64.
	//(h << 6 is just a prettier way of writting h*64)
	FT_Set_Char_Size(face, h << 6, h << 6, 96, 96);

	//Here we ask opengl to allocate resources for
	//all the textures and displays lists which we
	//are about to create.  
	ListBase = glGenLists(MaxTextures);
	glGenTextures(MaxTextures, TexID);

	//This is where we actually create each of the fonts display lists.
	for(int i = 0; i < MaxTextures; i++)
		MakeDisplayList(face, i, ListBase, TexID);

	//We don't need the face information now that the display
	//lists have been created, so we free the assosiated resources.
	FT_Done_Face(face);

	//Ditto for the library.
	FT_Done_FreeType(library);

	Builded = true;

	return true;
}

bool CFreeTypeFonts::LoadFromMemory(BYTE* pFontDataBuf, UINT FontBufSize, UINT h, BYTE bBlendingMode, BYTE FilteringMode) 
{
	BlendingMode = bBlendingMode;
	FilterTexture = FilteringMode == FONT_FILTER_LINEAR;

	//Allocate some memory to store the texture ids.
	TexID = new GLuint[MaxTextures];

	this->FontHeight = (float)h;

	//Create and initilize a freetype font library.
	FT_Library library;
	if(FT_Init_FreeType(&library)) 
		return false;

	//The object in which Freetype holds information on a given
	//font is called a "face".
	FT_Face face;

	//This is where we load in the font information from the file.
	//Of all the places where the code might die, this is the most likely,
	//as FT_New_Face will die if the font file does not exist or is somehow broken.
	if(FT_New_Memory_Face(library, pFontDataBuf, FontBufSize, 0, &face))
		return false;

	//For some twisted reason, Freetype measures font size
	//in terms of 1/64ths of pixels.  Thus, to make a font
	//h pixels high, we need to request a size of h*64.
	//(h << 6 is just a prettier way of writting h*64)
	FT_Set_Char_Size(face, h << 6, h << 6, 96, 96);

	//Here we ask opengl to allocate resources for
	//all the textures and displays lists which we
	//are about to create.  
	ListBase = glGenLists(MaxTextures);
	glGenTextures(MaxTextures, TexID);

	//This is where we actually create each of the fonts display lists.
	for(int i = 0; i < MaxTextures; i++)
		MakeDisplayList(face, i, ListBase, TexID);

	//We don't need the face information now that the display
	//lists have been created, so we free the assosiated resources.
	FT_Done_Face(face);

	//Ditto for the library.
	FT_Done_FreeType(library);

	Builded = true;

	return true;
}

void CFreeTypeFonts::CreateBitmapPreview(char *FName, BYTE* pFontDataBuf, UINT FontBufSize, UINT h) 
{
	GLubyte* bmp_data = NULL;

	//Create and initilize a freetype font library.
	FT_Library library;
	if(FT_Init_FreeType(&library)) 
		return;

	//The object in which Freetype holds information on a given
	//font is called a "face".
	FT_Face face;

	//This is where we load in the font information from the file.
	//Of all the places where the code might die, this is the most likely,
	//as FT_New_Face will die if the font file does not exist or is somehow broken.
	if(FT_New_Memory_Face(library, pFontDataBuf, FontBufSize, 0, &face)){
		FT_Done_FreeType(library);
		return;
	}

	//For some twisted reason, Freetype measures font size
	//in terms of 1/64ths of pixels.  Thus, to make a font
	//h pixels high, we need to request a size of h*64.
	//(h << 6 is just a prettier way of writting h*64)
	FT_Set_Char_Size(face, h << 6, h << 6, 96, 96);
	
	int ch = 0;
	int Offset = 0;
	int BmpWidth  = 0;
	int BmpHeight = 0;
	for(ch = 0; ch < MaxTextures; ch++){
		//Load the Glyph for our character.
		if(FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT))
			goto ErrJmp;

		//Move the face's glyph into a Glyph object.
		FT_Glyph glyph;
		if(FT_Get_Glyph(face->glyph, &glyph))
			goto ErrJmp;

		//Convert the glyph to a bitmap.
		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

		//This reference will make accessing the bitmap easier
		FT_Bitmap& bitmap = bitmap_glyph->bitmap;

		//////////////////////////////////////

		BmpWidth += bitmap.width;
		if(bitmap.rows > BmpHeight)
			BmpHeight = bitmap.rows;

		//////////////////////////////////////

		FT_Done_Glyph(glyph);
	}

	//Allocate memory for the texture data.
	bmp_data = new GLubyte[BmpWidth * BmpHeight];
	
	for(ch = 0; ch < MaxTextures; ch++){
		//The first thing we do is get FreeType to render our character
		//into a bitmap.  This actually requires a couple of FreeType commands:

		//Load the Glyph for our character.
		if(FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT))
			goto ErrJmp;

		//Move the face's glyph into a Glyph object.
		FT_Glyph glyph;
		if(FT_Get_Glyph(face->glyph, &glyph))
			goto ErrJmp;

		//Convert the glyph to a bitmap.
		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

		//This reference will make accessing the bitmap easier
		FT_Bitmap& bitmap = bitmap_glyph->bitmap;

		//////////////////////////////////////
		
		for(int j = 0; j < BmpHeight; j++) {
			for(int i = 0; i < bitmap.width; i++){
				if(j < bitmap.rows){
					bmp_data[(i + BmpWidth * j)+Offset] = bitmap.buffer[i + bitmap.width * j];
				} else {
					bmp_data[(i + BmpWidth * j)+Offset] = 0;
				}				
			}
		}
		Offset += bitmap.width;
		//////////////////////////////////////

		FT_Done_Glyph(glyph);
	}

	{
		FILE *f = fopen(FName, "wb+");
		if(f){
			fwrite(&bmp_data[0], 1, BmpWidth * BmpHeight, f);
			fclose(f);
		}
	}

	ErrJmp:
	SAFE_DELETE_ARRAY(bmp_data);


	//We don't need the face information now that the display
	//lists have been created, so we free the assosiated resources.
	FT_Done_Face(face);

	//Ditto for the library.
	FT_Done_FreeType(library);
}

// Create a display list coresponding to the give character.
bool CFreeTypeFonts::MakeDisplayList(FT_Face face, BYTE ch, GLuint list_base, GLuint* tex_base) 
{

	//The first thing we do is get FreeType to render our character
	//into a bitmap.  This actually requires a couple of FreeType commands:

	//Load the Glyph for our character.
	if(FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT))
		return false;

	//Move the face's glyph into a Glyph object.
    FT_Glyph glyph;
    if(FT_Get_Glyph(face->glyph, &glyph))
		return false;

	//Convert the glyph to a bitmap.
	FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

	//This reference will make accessing the bitmap easier
	FT_Bitmap& bitmap = bitmap_glyph->bitmap;

	//Use our helper function to get the widths of
	//the bitmap data that we will need in order to create
	//our texture.
	int width  = NextPowerOf2(bitmap.width);
	int height = NextPowerOf2(bitmap.rows);

	//Allocate memory for the texture data.
	GLubyte* expanded_data = new GLubyte[2 * width * height];
	for(int Cpt = 0; Cpt < 2 * width * height; Cpt+=2){
		expanded_data[Cpt] = 0;
		expanded_data[Cpt+1] = 0;
	}
	

	//Here we fill in the data for the expanded bitmap.
	//Notice that we are using two channel bitmap (one for
	//luminocity and one for alpha), but we assign
	//both luminocity and alpha to the value that we
	//find in the FreeType bitmap. 
	//We use the ?: operator so that value which we use
	//will be 0 if we are in the padding zone, and whatever
	//is the the Freetype bitmap otherwise.

	// Orignial
	/*for(int j = 0; j < height; j++) {
		for(int i = 0; i < width; i++){
			expanded_data[2 * (i + j * width)] = expanded_data[2 * (i+j*width) + 1] = (i >= bitmap.width || j >= bitmap.rows) ? 0 : bitmap.buffer[i + bitmap.width * j];
		}
	}*/
	// My own version...
	for(int j = 0; j < height; j++) {
		for(int i = 0; i < width; i++){
			int bmpIndx = (i + bitmap.width * j);
			int datIndx = 2 * (i + j * width);
			
			if(i < bitmap.width && j < bitmap.rows){
				expanded_data[datIndx] = bitmap.buffer[bmpIndx];
			} else {
				expanded_data[datIndx] = 0;
			}			
			
			if(BlendingMode == BLEND_FONT_SPECIAL){

				if(expanded_data[datIndx] < 64){
					expanded_data[datIndx+1] = 0;
				} else {
					expanded_data[datIndx+1] = 255;
				}

				//expanded_data[datIndx+1] = 192 + (expanded_data[datIndx] / 4);

				/*double param, result;
				param  = 270.0 + (((double)expanded_data[datIndx]) / 255.0 * 90.0);
				result = -(sin(param*(PI/180.0)) * 255.0);
				expanded_data[datIndx+1] = 255-(BYTE)result;*/

			} else {
				expanded_data[datIndx+1] = expanded_data[datIndx];
			}
		}
	}
	// From updated version
	/*for(int j=0; j < height;j++){
		for(int i=0; i < width; i++) {
			expanded_data[2*(i+j*width)+1] = 255;
			expanded_data[2*(i+j*width)] =
					(i>=bitmap.width || j>=bitmap.rows) ?
					0 : bitmap.buffer[i + bitmap.width*j];
		}
	}*/
	//Now we just setup some texture paramaters.
    glBindTexture(GL_TEXTURE_2D, tex_base[ch]);
	if(FilterTexture){
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	//Here we actually create the texture itself, notice
	//that we are using GL_LUMINANCE_ALPHA to indicate that
	//we are using 2 channel data.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data);

	//With the texture created, we don't need to expanded data anymore
    SAFE_DELETE_ARRAY(expanded_data);

	//So now we can create the display list
	glNewList(list_base+ch, GL_COMPILE);

	glBindTexture(GL_TEXTURE_2D, tex_base[ch]);

	glPushMatrix();

	//first we need to move over a little so that
	//the character has the right amount of space
	//between it and the one before it.
	glTranslatef((float)bitmap_glyph->left, 0.0f, 0.0f);

	//Now we move down a little in the case that the
	//bitmap extends past the bottom of the line 
	//(this is only true for characters like 'g' or 'y'.
	glTranslatef(0.0f, (float)(bitmap_glyph->top - bitmap.rows), 0.0f);

	//Now we need to account for the fact that many of
	//our textures are filled with empty padding space.
	//We figure what portion of the texture is used by 
	//the actual character and store that information in 
	//the x and y variables, then when we draw the
	//quad, we will only reference the parts of the texture
	//that we contain the character itself.

	float x = (float)bitmap.width / (float)width;
	float y = (float)bitmap.rows  / (float)height;

	//Here we draw the texturemaped quads.
	//The bitmap that we got from FreeType was not 
	//oriented quite like we would like it to be,
	//so we need to link the texture to the quad
	//so that the result will be properly aligned.
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex2f(0.0f,                (float)bitmap.rows);
	glTexCoord2d(0, y); glVertex2f(0.0f,                0.0f);
	glTexCoord2d(x, y); glVertex2f((float)bitmap.width, 0.0f);
	glTexCoord2d(x, 0); glVertex2f((float)bitmap.width, (float)bitmap.rows);
	glEnd();
	glPopMatrix();

	FontWidth[ch] = (face->glyph->advance.x >> 6);
	glTranslatef((float)FontWidth[ch], 0.0f, 0.0f);

	//Finnish the display list
	glEndList();

	FT_Done_Glyph(glyph);

	return true;
}


void CFreeTypeFonts::Free()
{
	if(Builded){
		glDeleteLists(ListBase, MaxTextures);
		glDeleteTextures(MaxTextures, TexID);
		SAFE_DELETE_ARRAY(TexID);
		Builded = false;
	}
}


// 
UINT CFreeTypeFonts::CalcFirstVisibleCharsNum(char *pText, UINT MaxWidth)
{
	UINT TextWidth = 0;
	UINT TextLen   = strlen(pText);
	UINT FirstChar = 0;

	for(UINT Cpt = TextLen-1; Cpt >= 0; Cpt--){
		TextWidth += FontWidth[pText[Cpt]];
		
		if(TextWidth > MaxWidth){
			FirstChar = Cpt;
			break;
		}

		if(Cpt == 0)
			break;
	}

	return FirstChar;
}

// 
UINT CFreeTypeFonts::CalcNumCharsOverLimit(char *pText, UINT MaxWidth)
{
	UINT TextWidth = 0;
	UINT TextLen   = strlen(pText);
	UINT NumCharsToSkip = 0;

	for(UINT Cpt = 0; Cpt < TextLen; Cpt++){
		TextWidth += FontWidth[pText[Cpt]];
		if(TextWidth > MaxWidth)
			NumCharsToSkip++;
	}

	return NumCharsToSkip;
}

// 
UINT CFreeTypeFonts::CalcNumLines(char *pText, UINT MaxWidth)
{
	UINT TextBufferLen = strlen(&pText[0]);
	UINT CurrentLine   = 0;
	UINT NumCharToCopy = 0;
	UINT NumCharCopyed = 0;

	// Write the text, lines by lines
	while(NumCharCopyed < TextBufferLen){
		// Return the number of chars we can copy
		NumCharToCopy  = ParseTextToLine((BYTE*)&pText[NumCharCopyed], MaxWidth);
		NumCharCopyed += NumCharToCopy;
		CurrentLine++;

		// Avoid an infinite loop...
		if(NumCharCopyed == 0)
			break;
	}

	return CurrentLine;
}

// 
UINT CFreeTypeFonts::CalcCursorOffset(BYTE *pText)
{
	//Here we need to split some text into lines
	UINT TextLen = strlen((char*)pText);

	UINT TextWidth = 0;
	BOOL IsTextTooLong = FALSE;

	// Loop through the chars one by one...
	for(UINT Cpt = 0; Cpt < TextLen; Cpt++){
		// Accumulate the width taken
		TextWidth += FontWidth[pText[Cpt]];
	}		

	return TextWidth;
}

// 
UINT CFreeTypeFonts::ParseTextToLine(BYTE *pText, UINT MaxWidth)
{
	//Here we need to split some text into lines
	UINT TextLen = strlen((char*)pText);

	// Save the width of the chars taken
	UINT CroppedTextLen = 0;
	UINT TextWidth = 0;
	BOOL IsTextTooLong = FALSE;

	// Loop through the chars one by one...
	for(UINT Cpt = 0; Cpt < TextLen; Cpt++){
		
		// Accumulate the width taken
		TextWidth += FontWidth[pText[Cpt]];
		// Increment the chars counter
		CroppedTextLen++;
		
		// If the width limit is reached...
		if(TextWidth > MaxWidth){
			IsTextTooLong = TRUE;
			break;
		}
	}		

	if(CroppedTextLen == 0)
		return 0;
	
	UINT LastCharPos  = CroppedTextLen - 1;
	BOOL IsSpace = pText[LastCharPos] == 0x20;

	if(IsTextTooLong){
		if(IsSpace){
			while(pText[LastCharPos] == 0x20){
				LastCharPos--;		
				if(LastCharPos == 0){
					LastCharPos = CroppedTextLen - 1;
					break;
				}
			}
		} else {
			while(pText[LastCharPos] != 0x20){
				LastCharPos--;		
				if(LastCharPos == 0){
					LastCharPos = CroppedTextLen - 1;
					break;
				}
			}
		}
	}
	
	return LastCharPos + 1;
}

/*void CFreeTypeFonts::PrintTxt(float x, float y, float sx, float sy, float r, float g, float b, const char *text)
{
	PrintArg(false, x, y, sx, sy, r, g, b, "%s", text);
}*/

void CFreeTypeFonts::PrintArg(UINT MaxWidth, UINT BufferSize, bool Rescale, const char *fmt, ...)
{
	float x  = (float)Settings.L;
	float y  = (float)Settings.T;
	float sx = Settings.SX;
	float sy = Settings.SY;
	
	BYTE r = Settings.R;
	BYTE g = Settings.G;
	BYTE b = Settings.B;

	GLuint font = ListBase;
	
	float h = FontHeight/* * 0.90f*/;						//We make the height about 1.5* that of

	static char DefaultBuffer[2048];

	// If the lenght, reuse the buffer for better speed, else, allocate a new buffer
	char *text = NULL;
	if(BufferSize == 0){
		ZeroMemory(&DefaultBuffer[0], 2048);
		text = &DefaultBuffer[0];
	} else {
		text = new char[BufferSize];
	}

	va_list ap;										// Pointer To List Of Arguments
	if(fmt == NULL){									// If There's No Text
		*text = 0;											// Do Nothing
	} else {
		va_start(ap, fmt);									// Parses The String For Variables
			vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
		va_end(ap);											// Results Are Stored In Text
	}

	//////////////////////////////////////////////////////////////////////////////////////

	Print(MaxWidth, BufferSize, Rescale, x, y, sx, sy, r, g, b, text);
	
	//////////////////////////////////////////////////////////////////////////////////////

	// Desallocate the buffer
	if(BufferSize != 0){
		SAFE_DELETE_ARRAY(text);
	}
}


///Much like Nehe's glPrint function, but modified to work
///with freetype fonts.
void CFreeTypeFonts::PrintArg(UINT MaxWidth, UINT BufferSize, bool Rescale, float x, float y, float sx, float sy, float r, float g, float b, const char *fmt, ...)  
{
	GLuint font = ListBase;
	
	float h = FontHeight/* * 0.90f*/;						//We make the height about 1.5* that of

	static char DefaultBuffer[2048];

	// If the lenght, reuse the buffer for better speed, else, allocate a new buffer
	char *text = NULL;
	if(BufferSize == 0){
		ZeroMemory(&DefaultBuffer[0], 2048);
		text = &DefaultBuffer[0];
	} else {
		text = new char[BufferSize];
	}

	va_list ap;										// Pointer To List Of Arguments
	if(fmt == NULL){									// If There's No Text
		*text = 0;											// Do Nothing
	} else {
		va_start(ap, fmt);									// Parses The String For Variables
			vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
		va_end(ap);											// Results Are Stored In Text
	}

	//////////////////////////////////////////////////////////////////////////////////////
	
	Print(MaxWidth, BufferSize, Rescale, x, y, sx, sy, r, g, b, text);
	
	//////////////////////////////////////////////////////////////////////////////////////

	// Desallocate the buffer
	if(BufferSize != 0){
		SAFE_DELETE_ARRAY(text);
	}
}

void CFreeTypeFonts::Print(UINT MaxWidth, UINT BufferSize, bool Rescale, float x, float y, float sx, float sy, float r, float g, float b, const char *text)  
{
	// We want a coordinate system where things coresponding to window pixels.
	PushScreenCoordinateMatrix();					
	
	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT);	
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	// Setup blending
	//if(BlendingMode == BLEND_FONT_NORMAL){
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	/*} else if(BlendingMode == BLEND_FONT_SPECIAL){
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_DST_ALPHA);	
	}*/
	glColor3f(r,g,b);

	glListBase(ListBase);

	float modelview_matrix[16];	
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	glPushMatrix();
	glLoadIdentity();
	glScalef(1.0f, -1.0f, 1.0f);
	if(Rescale)
		glScalef(sx, sy, 1.0f);
	glTranslatef(x, -y - FontHeight, 0.0f);
	glMultMatrixf(modelview_matrix);

	//////////////////////////////////////////////////////////////////////////////////////

	// If we're drawing all one one line...
	if(MaxWidth == 0){

		glCallLists(strlen(&text[0]), GL_UNSIGNED_BYTE, &text[0]);

	} else { // if MultiLines...
	
		//Here we need to split some text into lines

		UINT TextBufferLen = strlen(&text[0]);
		UINT CurrentLine   = 0;
		UINT NumCharToCopy = 0;
		UINT NumCharCopyed = 0;

		// Write the text, lines by lines
		while(NumCharCopyed < TextBufferLen){
		
			// Return the number of chars we can copy
			NumCharToCopy = ParseTextToLine((BYTE*)&text[NumCharCopyed], MaxWidth);

			// Draw those on screen
			glCallLists(NumCharToCopy, GL_UNSIGNED_BYTE, &text[NumCharCopyed]);

			// Update counters
			NumCharCopyed += NumCharToCopy;
			CurrentLine++;

			// Avoid an infinite loop...
			if(NumCharCopyed == 0){
				glLoadIdentity();
				break;
			}

			// This "change" the text position to write the next line of text
			glLoadIdentity();
			glScalef(1.0f, -1.0f, 1.0f);
			if(Rescale)
				glScalef(sx, -sy, 1.0f);
			glTranslatef(x, -y - (FontHeight*(float)(CurrentLine+1)), 0.0f);
		}
	}

	glPopMatrix();
	glPopAttrib();		
	PopProjectionMatrix();
}

#endif
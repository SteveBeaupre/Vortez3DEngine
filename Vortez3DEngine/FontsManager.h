#ifndef _CFONTS_MANAGER_H_
#define _CFONTS_MANAGER_H_
#ifdef __cplusplus
//----------------------------------------------------------------------//
#define EXP_FUNC __declspec(dllexport)
//----------------------------------------------------------------------//
#ifdef COMPILE_FOR_DIRECTX9
#else 
#define COMPILE_FOR_OPENGL
#endif
//----------------------------------------------------------------------//
#define WIN32_LEAN_AND_MEAN // Trim libraies.
#define VC_LEANMEAN         // Trim even further.
#include "Windows.h"
#include "stdio.h"
//----------------------------------------------------------------------//
#ifdef COMPILE_FOR_OPENGL
#include "BitmapFonts.h"
#include "FreeTypeFonts.h"
#endif
#include "GameWindow.h"
#include "Extern.h"
#include "SafeKill.h"
//----------------------------------------------------------------------//

#ifndef FONTS_MGR_ROOT_NODE
	#define FONTS_MGR_ROOT_NODE    -1
#endif

#define BITMAP_FONT    0
#define FREETYPE_FONT  1

struct CFontData {
	UINT Type;
	char *pText;
	char FontName[256];
	BYTE Color[3];
	BYTE Size;
	int  Left,Top;
	bool Bold,Italic;
};

struct CFontNode {
	UINT Indx;

	// To customize...
	UINT FontID;
	char *pText;
	void* pFont;
	CFontData Data;

	//BOOL IsSelected;

	struct CFontNode *Prev;
	struct CFontNode *Next;
};


class EXP_FUNC CFontsManager {
public:
	CFontsManager();
	~CFontsManager();
private:
	UINT NumNodesAllocated;

	CFontNode Root;
	void InitRootNode();
	CFontNode* GetRootNode();
	void RecreateIndex();
public:
	UINT GetNodesCount();
	UINT GetNumAllocatedNodes(){return NumNodesAllocated;}

	CFontNode* GetNode(UINT Indx);
	CFontNode* GetFirstNode(bool CanReturnRoot = false);
	CFontNode* GetLastNode(bool CanReturnRoot = false);

	CFontNode* Push(char *pName, CFontData *pFontData);
	bool Delete(int Indx);
	bool Pop();
	void Clear();
};

#endif
#endif //--_CFONTS_MANAGER_H_

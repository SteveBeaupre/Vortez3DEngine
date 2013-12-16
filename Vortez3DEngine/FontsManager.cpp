#include "FontsManager.h"

/////////////////////////////////////////////////////////////////////////////////////////////////

CFontsManager::CFontsManager()
{
	NumNodesAllocated = 0;
	InitRootNode();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CFontsManager::~CFontsManager()
{
	Clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CFontsManager::InitRootNode()
{
	Root.Indx = FONTS_MGR_ROOT_NODE;
	Root.FontID = 0;
	Root.pText = NULL;
	Root.pFont = NULL;
	ZeroMemory(&Root.Data, sizeof(CFontData));
	//Root.IsSelected = FALSE;

	Root.Prev = NULL;
	Root.Next = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CFontsManager::RecreateIndex()
{
	UINT Count = 0;
	CFontNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		pNode->Indx = Count;
		Count++;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

UINT CFontsManager::GetNodesCount()
{
	UINT Count = 0;
	CFontNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		Count++;
	}

	// Return the number of nodes of this pile
	return Count;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CFontNode* CFontsManager::GetNode(UINT Indx)
{
	UINT Count = 0;
	CFontNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the one we want
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		if(Count == Indx)
			return pNode;
		Count++;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CFontNode* CFontsManager::GetRootNode()
{
	return &Root;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CFontNode* CFontsManager::GetFirstNode(bool CanReturnRoot)
{
	CFontNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	if(pNode->Next != NULL)
		pNode = pNode->Next;

	if(CanReturnRoot)
		return pNode;
	
	// Return the last node pointer
	return pNode == &Root ? NULL : pNode;
}


/////////////////////////////////////////////////////////////////////////////////////////////////

CFontNode* CFontsManager::GetLastNode(bool CanReturnRoot)
{
	CFontNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL)
		pNode = pNode->Next;

	if(CanReturnRoot)
		return pNode;

	// Return the last node pointer
	return pNode == &Root ? NULL : pNode;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CFontNode* CFontsManager::Push(char *pName, CFontData *pFontData)
{
	CFontNode *pNode = GetLastNode(true);

	// Allocate the new node
	pNode->Next = new CFontNode;
	NumNodesAllocated++;
	CFontNode *pNewNode = pNode->Next;

	// Update pointers
	pNewNode->Next = NULL;
	pNewNode->Prev = pNode;

	//
	pNewNode->Indx = pNewNode->Prev->Indx + 1;
	//pNewNode->IsSelected = FALSE;

	// To customize...
	memcpy(&pNewNode->Data, pFontData, sizeof(CFontData));

	if(pNewNode->Data.pText != NULL){
		int TexLen = strlen(pNewNode->Data.pText);
		if(TexLen != 0){
			pNewNode->pText = new char[TexLen+1];
			memcpy(pNewNode->pText, pNewNode->Data.pText, TexLen);
			pNewNode->pText[TexLen] = NULL;
		} else {
			pNewNode->pText = new char[1];
			pNewNode->pText[0] = NULL;
		}
	} else {
		pNewNode->pText = NULL;
	}

	pNewNode->pFont = NULL;
	#ifdef COMPILE_FOR_DIRECTX9
	#else
		switch(pFontData->Type)
		{
		case BITMAP_FONT:    
			{
				CBitmapFonts *font = new CBitmapFonts;
				pNewNode->pFont = font;

				CBitmapFonts::CBitmapFontSettings Settings;
				Settings.hDC = GetDC(pGameWindow->GetHWND());
				Settings.FontName = &pFontData->FontName[0];
				Settings.FontSize = pFontData->Size;
				Settings.l = pFontData->Left;
				Settings.t = pFontData->Top;
				Settings.r = pFontData->Color[0];
				Settings.g = pFontData->Color[1];
				Settings.b = pFontData->Color[2];
				font->Build(&Settings);
			}
			break;
		case FREETYPE_FONT:    
			{
				CFreeTypeFonts *font = new CFreeTypeFonts;
				pNewNode->pFont = font;

				CFreeTypeFonts::CFreeTypeFontSettings Settings;
				Settings.L = pFontData->Left;
				Settings.T = pFontData->Top;
				Settings.SX = 1.0f;
				Settings.SY = 1.0f;
				Settings.R = pFontData->Color[0];
				Settings.G = pFontData->Color[1];
				Settings.B = pFontData->Color[2];
				font->SetSettings(&Settings);

				font->LoadFromFile(&pFontData->FontName[0], pFontData->Size, BLEND_FONT_NORMAL, FONT_FILTER_LINEAR);

				// Extract and save the file name (without extention)
				char Tmp[256];
				ZeroMemory(&Tmp[0], 256);
				int Indx = strlen(&pFontData->FontName[0]) - 1;
				while(pFontData->FontName[Indx] != '\\'){
					Indx--;
				}
				Indx++;
				int Cpt = 0;
				while(pFontData->FontName[Indx] != '.'){
					Tmp[Cpt++] = pFontData->FontName[Indx];
					Indx++;
					if(Indx >= 256)
						break;
				}
				strncpy(&pFontData->FontName[0], &Tmp[0], 255);
			}
			break;
		}
	#endif

	return pNewNode;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool CFontsManager::Delete(int Indx)
{
	CFontNode *pNode = GetNode(Indx);

	// If this the root node, we're done...
	if(!pNode || pNode == &Root){return false;} // return faillure

	// Update pointers
	CFontNode *pPrevNode = pNode->Prev;
	CFontNode *pNextNode = pNode->Next;

	pPrevNode->Next  = pNextNode;
	if(pNextNode)
		pNextNode->Prev = pPrevNode;

	// Free the allocated font
	if(pNode->pFont != NULL){
		SAFE_DELETE_ARRAY(pNode->pText);
		#ifdef COMPILE_FOR_DIRECTX9
		#else
			switch(pNode->Data.Type)
			{
			case BITMAP_FONT:   
				{
					CBitmapFonts* font = (CBitmapFonts*)pNode->pFont;   
					SAFE_DELETE_OBJECT(font);
				}
				break;    
			case FREETYPE_FONT:	
				{
					CFreeTypeFonts* font = (CFreeTypeFonts*)pNode->pFont; 
					SAFE_DELETE_OBJECT(font);
				}
				break;
			}
		#endif
	}

	// Delete the node
	SAFE_DELETE_OBJECT(pNode);
	NumNodesAllocated--;

	RecreateIndex();

	// return success
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool CFontsManager::Pop()
{
	CFontNode *pNode = GetLastNode(true);

	// If this the root node, we're done...
	if(pNode == NULL || pNode == &Root){return false;} // return faillure

	// Free the allocated font
	if(pNode->pFont != NULL){
		SAFE_DELETE_ARRAY(pNode->pText);
		#ifdef COMPILE_FOR_DIRECTX9
		#else
			switch(pNode->Data.Type)
			{
			case BITMAP_FONT:   
				{
					CBitmapFonts* font = (CBitmapFonts*)pNode->pFont;   
					SAFE_DELETE_OBJECT(font);
				}
				break;    
			case FREETYPE_FONT:	
				{
					CFreeTypeFonts* font = (CFreeTypeFonts*)pNode->pFont; 
					SAFE_DELETE_OBJECT(font);
				}
				break;
			}
		#endif
	}

	// Update pointers
	pNode->Prev->Next = NULL;

	// Delete the node
	SAFE_DELETE_OBJECT(pNode);
	NumNodesAllocated--;

	// return success
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CFontsManager::Clear()
{
	// Delete everything...
	while(Pop());
}


#include "Sprite.h"

CSprite::CSprite()
{
	#ifdef COMPILE_FOR_DIRECTX9
	#else
		TexID = 0;
	#endif
	FitScreen = false;
	Rescale = false;

	pIndexArray = &IndexArray[0];
	pVertsArray = &VertsArray[0];
	pTexVertsArray = &TexVertsArray[0];

	ZeroMemory(pIndexArray,    sizeof(UINT)   * NUM_SPRITE_VERTS);
	ZeroMemory(pVertsArray,    sizeof(float2) * NUM_SPRITE_VERTS);
	ZeroMemory(pTexVertsArray, sizeof(float2) * NUM_SPRITE_VERTS);
}

void CSprite::GetWindowSize(float *w, float *h)
{
	RECT r;
	GetClientRect(pGameWindow->GetHWND(), &r);
	*w = (float)(r.right - r.left);
	*h = (float)(r.bottom - r.top);
}

#ifdef COMPILE_FOR_DIRECTX9
#else
void CSprite::Set(CQuadRect *pCoords, float4 *pTexCoords, UINT TexID, bool bFitScreen, bool bRescale)
{
	memcpy(&Coords, pCoords, sizeof(CQuadRect));
	if(pTexCoords)
		memcpy(&TexCoords, pTexCoords, sizeof(float4));

	FitScreen = bFitScreen;
	Rescale = bRescale;

	float l,r,t,b;
	float TexL,TexW,TexT,TexH;
	
	/*if(Rescale && !FitScreen){
		// Calculate Vertex coords
		l = pCoords->L;
		r = pCoords->L + pCoords->W;
		t = pCoords->T;
		b = pCoords->T + pCoords->H;
	} else {*/
		// Calculate Vertex coords
		l = pCoords->L;
		r = pCoords->L + pCoords->W;
		t = pCoords->T;
		b = pCoords->T + pCoords->H;
	//}

	// Calculate Texture coords
	if(pTexCoords){
		TexL = pTexCoords->x;
		TexW = pTexCoords->y;
		TexT = pTexCoords->z;
		TexH = pTexCoords->a;
	} else {
		TexL = 0.0f;
		TexW = 1.0f;
		TexT = 0.0f;
		TexH = 1.0f;
	}

	// Setup vertex buffers
	float Coords[2][NUM_SPRITE_VERTS] = {{l, r, r, l}, {t, t, b, b}};
	float TexCoords[2][NUM_SPRITE_VERTS] = {{TexL, TexL + TexW, TexL + TexW, TexL}, {TexT + TexH, TexT + TexH, TexT, TexT}};
	// Fill the arrays
	for(int Cpt = 0; Cpt < NUM_SPRITE_VERTS; Cpt++){
		IndexArray[Cpt] = Cpt;
		VertsArray[Cpt].x = Coords[0][Cpt];
		VertsArray[Cpt].y = Coords[1][Cpt];
		TexVertsArray[Cpt].x = TexCoords[0][Cpt];
		TexVertsArray[Cpt].y = TexCoords[1][Cpt];
	}

	SetTexture(TexID);
}

void CSprite::SetTexture(UINT id)
{
	TexID = id;
}
#endif

#ifdef COMPILE_FOR_OPENGL
void CSprite::EnableVAs(bool UseTexture)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	if(UseTexture)
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void CSprite::DisableVAs(bool UseTexture)
{
	glDisableClientState(GL_VERTEX_ARRAY);
	if(UseTexture)
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void CSprite::BindVAs(bool UseTexture)
{
	glVertexPointer(2, GL_FLOAT, 0, pVertsArray);
	if(UseTexture)
		glTexCoordPointer(2, GL_FLOAT, 0, pTexVertsArray);
}
#endif

void CSprite::Draw()
{
	#ifdef COMPILE_FOR_DIRECTX9
	#else
		if(FitScreen){
			float w, h;
			GetWindowSize(&w, &h);
			CQuadRect Rect(0.0f, 0.0f, w, h);
			Set(&Rect, &TexCoords, TexID, FitScreen, Rescale);
		}

		bool UseTexture = TexID != 0;
		if(UseTexture)
			glBindTexture(GL_TEXTURE_2D, TexID);

		EnableVAs(UseTexture);
		BindVAs(UseTexture);
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, pIndexArray);
		DisableVAs(UseTexture);
	#endif
}

#ifndef _SPRITE_H_
#define _SPRITE_H_
#ifdef __cplusplus
//----------------------------------------------------------------------//
#ifdef COMPILE_FOR_DIRECTX9
#else 
#define COMPILE_FOR_OPENGL
#endif
//----------------------------------------------------------------------//
#define EXP_FUNC __declspec(dllexport)
//----------------------------------------------------------------------//
#include "Windows.h"
#include "Stdio.h"
//----------------------------------------------------------------------//
#include <gl\gl.h>
//----------------------------------------------------------------------//
#include "Vortez3DTypes.h"
#include "Texture.h"
#include "GameWindow.h"
//----------------------------------------------------------------------//
#include "Extern.h"
//----------------------------------------------------------------------//

#define NUM_SPRITE_VERTS  4

class EXP_FUNC CQuadRect{
public:
	float L, T, W, H;
	CQuadRect(){SetData(0.0f, 0.0f, 0.0f, 0.0f);}
	CQuadRect(float *pArray){SetData(pArray);}
	CQuadRect(float l, float t, float w, float h){SetData(l, t, w, h);}
	void SetData(float l, float t, float w, float h){L = l; T = t; W = w; H = h;}
	void SetData(float *pArray){L = pArray[0]; T = pArray[1]; W = pArray[2]; H = pArray[3];}
	void GetData(float *l, float *t, float *w, float *h){*l = L; *t = T; *w = W; *h = H;}
};

//----------------------------------------------------------------------//
//-------------------CSprite Class Definition---------------------------//
//----------------------------------------------------------------------//
class EXP_FUNC CSprite {
public:
	CSprite();
private:
	void GetWindowSize(float *w, float *h);
private:
	UINT   IndexArray[NUM_SPRITE_VERTS];	
	float2 VertsArray[NUM_SPRITE_VERTS];	
	float2 TexVertsArray[NUM_SPRITE_VERTS];	
public:
	UINT   *pIndexArray;	
	float2 *pVertsArray;	
	float2 *pTexVertsArray;	
private:
#ifdef COMPILE_FOR_DIRECTX9
#else
	UINT TexID;
#endif
	bool Rescale;
	bool FitScreen;
	CQuadRect Coords;
	float4 TexCoords;
public:
#ifdef COMPILE_FOR_OPENGL
	void EnableVAs(bool UseTexture);
	void DisableVAs(bool UseTexture);
	void BindVAs(bool UseTexture);
#endif
public:
#ifdef COMPILE_FOR_DIRECTX9
#else
	void Set(CQuadRect *pCoords, float4 *pTexCoords = NULL, UINT TexID = 0, bool bFitScreen = false, bool bRescale = true);
	void SetTexture(UINT id);
#endif
	void Draw();
};

#endif
#endif //--_SPRITE_H_

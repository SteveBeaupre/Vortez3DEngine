#ifndef _CVORTEZ3D_ENGINE_H_
#define _CVORTEZ3D_ENGINE_H_
#ifdef __cplusplus
//----------------------------------------------------------------------//
#define EXP_FUNC __declspec(dllexport)
//----------------------------------------------------------------------//
#ifdef COMPILE_FOR_DIRECTX9
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#else
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#define COMPILE_FOR_OPENGL
//#define COMPILE_FOR_OPENGL_3_1_
#ifdef COMPILE_FOR_OPENGL_3_1_
#pragma comment(lib, "glew32sd.lib")
#endif
#endif
//----------------------------------------------------------------------//
#define WIN32_LEAN_AND_MEAN 
#define VC_LEANMEAN         
//----------------------------------------------------------------------//
#pragma comment(lib, "VortezSoundEngine.lib")
#pragma comment(lib, "VortezNetworkEngine.lib")
#pragma comment(lib, "VortezSDK.lib")
#include <VortezSoundEngine.h>
#include <VortezNetworkEngine.h>
#include <VortezSDK.h>
//----------------------------------------------------------------------//
#include "Windows.h"
#include "Stdio.h"
//----------------------------------------------------------------------//
#ifdef COMPILE_FOR_DIRECTX9
#include <d3d9.h>
#include <d3dx9.h>
#else
#ifdef COMPILE_FOR_OPENGL_3_1_
#define GLEW_STATIC
#include <gl\glew.h>
#include <gl\wglew.h>
#endif
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glext.h>
#endif
//----------------------------------------------------------------------//
#ifdef COMPILE_FOR_OPENGL
#include "GLCompatibility.h"
#include "Shader.h"
#include "FBO.h"
#include "VBO.h"
#endif
#include "Vortez3DTypes.h"
#include "GameWindow.h"
#include "Timer.h"
#include "Camera.h"
#include "ModelsManager.h"
#include "TexturesManager.h"
#include "SpritesManager.h"
#include "FontsManager.h"
#include "Log.h"
#include "SafeKill.h"
//----------------------------------------------------------------------//

#define LEFT_MOUSE_BUTTON    0
#define MIDDLE_MOUSE_BUTTON  1
#define RIGHT_MOUSE_BUTTON   2

#ifdef COMPILE_FOR_OPENGL
	// OpenGL VSync Extension
	typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
#endif

#define USE_BMP_FPS 

// Message pump foward declaration
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#ifdef COMPILE_FOR_OPENGL_3_1_
class EXP_FUNC CShaderExt : public CShader {
private:
	UINT LightLoc, TexLoc;
public:
	void InitShaderDataExt();
};
#endif

//----------------------------------------------------------------------//
//------------------------The Main Class--------------------------------//
//----------------------------------------------------------------------//
class EXP_FUNC CVortez3DEngine {
public:
	CVortez3DEngine();
	~CVortez3DEngine();
public:
	CModelsManager      ModelsManager;
	CSpritesManager     SpritesManager;
	CFontsManager       FontsManager;
	CTexturesManagerExt TexturesManager;
protected:
#ifdef COMPILE_FOR_OPENGL
	CGLCompatibility GLCompatibility;
#endif
private:
	#ifdef COMPILE_FOR_DIRECTX9
		LPDIRECT3D9 d3d;
		LPDIRECT3DDEVICE9 d3ddev;
	#else
		CVBO VBO;
	#endif
private:
	CGameTimer EngineTimer;
private:
	BOOL IsEngineInitialized;
#ifdef COMPILE_FOR_OPENGL
	BOOL IsOpenGLForLoadingInitialized;
#endif
private:
	HINSTANCE hInstance; 
private:
#ifdef COMPILE_FOR_OPENGL
	HDC   hDC;       
	HGLRC hRC;	    
	HDC   hLoadingDC;
	HGLRC hLoadingRC;
#endif
public:
	CGameWindow GameWindow;
private:
	bool MouseClamped;
private:
	bool KeyState[256];
	bool MouseBtnState[3];
private:
#ifdef COMPILE_FOR_DIRECTX9
	LPD3DXFONT d3dFPSFont;
#else
	#ifdef USE_BMP_FPS
	CBitmapFonts FPSFont;
	#else
	CFreeTypeFonts FPSFont;
	#endif
#endif
private:
#ifdef COMPILE_FOR_OPENGL
	bool FogExtSupported;
	bool ShaderSupported;
	bool MultitextureExtSupported;
	int  NumTexelUnits;
#endif
private:
	__int64 NumPolygonsDrawed;
private:
	bool UseEscKeyToQuit;
private:
	void ProcessMessages();
	void GameLoop();
private:
#ifdef COMPILE_FOR_DIRECTX9
	bool InitializeDirectX9();
	void ShutdownDirectX9();
#else
	bool SetupPixelFormatDescriptor(HDC hdc);
	bool InitializeOpenGL();
	void ShutdownOpenGL();
#endif
private:
	void OnCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef COMPILE_FOR_OPENGL
	PFNWGLSWAPINTERVALFARPROC SwapIntervalEXT;
#endif
#ifdef COMPILE_FOR_OPENGL_3_1_
	CShader DefaultShader;
#endif
public:
	HINSTANCE GethInstance(){return hInstance;}
	HWND      GethWnd     (){return GameWindow.GetHWND();}
	#ifdef COMPILE_FOR_OPENGL
		HDC       GethDC      (){return hDC;}
		HGLRC     GethGLRC    (){return hRC;}
		HDC       GetLoadinghDC  (){return hLoadingDC;}
		HGLRC     GetLoadinghGLRC(){return hLoadingRC;}
	#endif
public:
#ifdef COMPILE_FOR_DIRECTX9
	LPDIRECT3D9       GetD3D(){return d3d;}
	LPDIRECT3DDEVICE9 GetDevice(){return d3ddev;}
#endif
public:
	bool InitEngine(CInitInfo *pInitInfo);
	void StartEngine();
	void ShutdownEngine();
public:
#ifdef COMPILE_FOR_OPENGL
	bool IsLoadingDone();
	bool InitializeOpenGLForLoading();
	void CleanUpOpenGLForLoading();
	void RenderLoading(int PercentDone);
#endif
public:
	__int64 GetPolygonsDrawed(){return NumPolygonsDrawed;}
public:
	bool GetUseEscKeyToQuit(){return UseEscKeyToQuit;}
	void SetUseEscKeyToQuit(bool bUseEscKeyToQuit){UseEscKeyToQuit = bUseEscKeyToQuit;}
public:
	CCamera Camera;
public:
	bool GetKeyStatus(BYTE key){return KeyState[key];}
	bool GetMouseBtnStatus(BYTE mb){return MouseBtnState[mb];}
public:
	#ifdef COMPILE_FOR_OPENGL
		bool  IsVBOSupported();
		CVBO* GetVBOPtr();

		bool IsFogExtSupported(){return FogExtSupported;}
		bool IsShadersSupported(){return ShaderSupported;}
		bool IsMultitextureExtSupported(){return MultitextureExtSupported;}
		int  GetTexelUnitNumber(){return NumTexelUnits;}
		
		void SetVSync(int i){if(SwapIntervalEXT)SwapIntervalEXT(i);}
	#endif
	#ifdef COMPILE_FOR_DIRECTX9
		void DrawQuadsDX(float Left, float Top, float Width, float Height);
	#else
		void DrawQuads(float Left, float Top, float Width, float Height, float4 *pTexCoords = NULL, bool Rescale = true, bool HCentered = false, bool VCentered = false);
		void DrawQuads(CQuadRect *pCoords, float4 *pTexCoords = NULL, bool Rescale = true, bool HCentered = false, bool VCentered = false);
		void DrawTexQuads(UINT TexID, float Left, float Top, float Width, float Height, float4 *pTexCoords = NULL, bool Rescale = true, bool HCentered = false, bool VCentered = false);
		void DrawTexQuads(UINT TexID, CQuadRect *pCoords, float4 *pTexCoords = NULL, bool Rescale = true, bool HCentered = false, bool VCentered = false);

		UINT GetCurrentTextureUnit();
		void SetCurrentTextureUnit(UINT Val);
	#endif
	#ifdef COMPILE_FOR_OPENGL
		void SetPolygonMode(GLenum Mode){glPolygonMode(GL_FRONT, Mode);}
	#endif
public:
	void ClampMouse(bool b){MouseClamped = b;}
public:
	float GetFPS(){return EngineTimer.GetFPS();}
	void  DrawFPS();
	void  DrawFPS(bool Rescale, int *pVPCoords, float r = 1.0f, float g = 1.0f, float b = 0.0f);
public:
	bool LoadFont(char *fname){return AddFontResource(fname) > 0;}
	void FreeFont(char *fname){RemoveFontResource(fname);}
public:
	int   InvYi(int y){return GameWindow.GetHeight() - y;}
	float InvYf(float y){return GameWindow.GetHeightf() - y;}
public:
	void TimeStuff(float *var, float ElapsedTime, int TimeScale, float ElpasedTimeRatio, float Limit);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	virtual void OnCustomMessageExt(UINT uMsg, WPARAM wParam, LPARAM lParam){}	
public:
	virtual void PlaySoundFXExt(int SoundID){}
	virtual void PlayTuneExt(int TuneID){}
public:
	virtual void OnWinsockMsg(UINT uMsg, WPARAM wParam, LPARAM lParam){}	
	virtual void OnNetworkMsg(UINT uMsg, WPARAM wParam, LPARAM lParam){}	
public:
	virtual void OnChar(UINT Ch){}
	virtual void OnKeyDown(UINT KeyPressed){}
	virtual void OnKeyUp(UINT KeyReleased){}
public:
	virtual void OnMouseDown(int ButtonPressed, int x, int y){}
	virtual void OnMouseUp(int ButtonPressed, int x, int y){}
	virtual void OnMouseMove(int x, int y);
	virtual void OnMouseMove(int x, int y, int XDisp, int YDisp){}
	virtual void OnMouseRoll(int RollCount){}
public:
	virtual void OnGameWindowResized(int w, int h){}
public:
	virtual void InitEngineExt(){}
	virtual void ShutdownEngineExt(){}
public:
	#ifdef COMPILE_FOR_OPENGL
	virtual void OnInitializeOpenGLForLoadingExt(){}
	virtual void OnCleanUpOpenGLForLoadingExt(){}
	#endif
public:
	virtual void RenderScene(){}
	virtual void RenderLoadingExt(int PercentDone){}
	virtual void UpdateScene(float ElapsedTime){}
};

#endif
#endif //_CVORTEZ3D_ENGINE_H_

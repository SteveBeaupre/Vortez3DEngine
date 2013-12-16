#ifndef _GAME_WINDOW_H_
#define _GAME_WINDOW_H_
#ifdef __cplusplus
//----------------------------------------------------------------------//
#define EXP_FUNC __declspec(dllexport)
//----------------------------------------------------------------------//
#include "Windows.h"
#include "Stdio.h"
//----------------------------------------------------------------------//

//----------------------------------------------------------------------//
//------------------CInitInfo Class Definition--------------------------//
//----------------------------------------------------------------------//
struct EXP_FUNC CDevSettings { // Should remove that crap...
	int   Width, Height;
	float fWidth, fHeight;
};

struct EXP_FUNC CInitInfo {
	HINSTANCE hInst;
	HWND wndhandle;

	char ClassName[256];
	char WindowCaption[256];
	WORD IconID;
	WORD IconSmID;
	char IconName[256];
	char IconSmName[256];

	int  Left, Top, Width, Height;
	bool FullScreen;
	bool WideScreen;
	bool StartMaximized;
	bool ManageCursor;
};

//----------------------------------------------------------------------//
//------------------The Window Class Definition-------------------------//
//----------------------------------------------------------------------//
class EXP_FUNC CGameWindow {
public:
	CGameWindow();
	~CGameWindow();
private:
	HWND      m_hWnd;
	HINSTANCE m_hInstance;
	bool IsWindowAttached;
	bool AttachWindow(HWND hRendererWnd);
private:
	char szCaption[256];
	char szClassName[256];

	BOOL Registered;
	BOOL FullScreen;
	BOOL WideScreen;
	
	BOOL CanResizeScreen;
	BOOL CanMoveScreen;

	RECT WindowRect;
	RECT ClientRect;

	BOOL ManageCursor;

	CDevSettings DevSettings;
public:
	bool Create(CInitInfo *pInitInfo, WNDPROC MsgProc);
	void Destroy();
public:
	bool IsFullScreen(){return FullScreen == 0 ? false : true;}
	bool IsWideScreen(){return WideScreen == 0 ? false : true;}
public:
	bool CanResize(){return CanResizeScreen == 0 ? false : true;}
	bool CanMove(){return CanMoveScreen == 0 ? false : true;}

	void DisableMove(bool b);
	void DisableResize(bool b);
public:
	HWND GetHWND(){return m_hWnd;}
public:
	RECT *GetGameWindowRect(){return &WindowRect;}
	RECT *GetGameClientRect(){return &ClientRect;}
	void ResizeToDesktopDimention();
	void UpdateRects();
public:
	char* GetCaption();
	char* GetClsName();
public:
	int   GetLeft()   {return ClientRect.left;}
	int   GetTop()    {return ClientRect.top;}
	int   GetWidth()  {return ClientRect.right - ClientRect.left;}
	int   GetHeight() {return ClientRect.bottom - ClientRect.top;}

	float GetLeftf()  {return (float)GetLeft();}
	float GetTopf()   {return (float)GetTop();}
	float GetWidthf() {return (float)GetWidth();}
	float GetHeightf(){return (float)GetHeight();}

	int   GetWndLeft()  {return WindowRect.left;}
	int   GetWndTop()   {return WindowRect.top;}
	int   GetWndWidth() {return WindowRect.right - WindowRect.left;}
	int   GetWndHeight(){return WindowRect.bottom - WindowRect.top;}
	
	float GetWndLeftf()  {return (float)GetWndLeft();}
	float GetWndTopf()   {return (float)GetWndTop();}
	float GetWndWidthf() {return (float)GetWndWidth();}
	float GetWndHeightf(){return (float)GetWndHeight();}

	float GetRatio()  {return GetWidthf() / GetHeightf();}
	float GetWndRatio(){return GetWndWidthf() / GetWndHeightf();}
public:
	int   GetDevWidth()  {return DevSettings.Width;}
	int   GetDevHeight() {return DevSettings.Height;}
	float GetDevWidthf() {return DevSettings.fWidth;}
	float GetDevHeightf(){return DevSettings.fHeight;}

	float GetWidthDevRatio() {return GetWidthf() /  DevSettings.fWidth;}
	float GetHeightDevRatio(){return GetHeightf() / DevSettings.fHeight;}

	void  SetDevSettings(int w, int h);
};

#endif
#endif //--_GAME_WINDOW_H_

#include "GameWindow.h"

CGameWindow::CGameWindow()
{
	m_hWnd = NULL;
	IsWindowAttached = false;

	ZeroMemory(&szCaption[0], 256);
	ZeroMemory(&szClassName[0], 256);

	Registered = false;
	FullScreen = true;

	ManageCursor = false;

	ZeroMemory(&WindowRect, sizeof(RECT));
	ZeroMemory(&ClientRect, sizeof(RECT));

	SetDevSettings(1024, 768);
}

CGameWindow::~CGameWindow()
{
	Destroy();
}

bool CGameWindow::AttachWindow(HWND hRendererWnd)
{
	IsWindowAttached = false;
	m_hWnd = NULL;

	if(hRendererWnd){
		m_hWnd = hRendererWnd;
		UpdateRects();
		GetCaption();
		GetClsName();

		IsWindowAttached = true;
	}

	return IsWindowAttached;
}

//-----------------------------------------------------------------------------
// Name : CreateGameWindow(CInitInfo *pInitInfo)
// Desc : Create the game window 
//-----------------------------------------------------------------------------
bool CGameWindow::Create(CInitInfo *pInitInfo, WNDPROC MsgProc)
{
	Destroy();
	if(AttachWindow(pInitInfo->wndhandle))
		return true;

	m_hInstance  = pInitInfo->hInst;
	ManageCursor = pInitInfo->ManageCursor;

	WNDCLASSEX wc;                            // Window class object.
	// This is the Window class.
	wc.cbSize        = sizeof(WNDCLASSEX);           // size of the WNDCLASSEX structure.
	wc.style         = CS_HREDRAW | CS_VREDRAW;       // style of the window.
	wc.lpfnWndProc   = MsgProc;                 // Address to the windows procedure.
	wc.cbClsExtra    = 0;                        // Extra class information.
	wc.cbWndExtra    = 0;                        // Extra window information.
	wc.hInstance     = m_hInstance; // Handle of application Instance.
	wc.hIcon         = pInitInfo->IconID == 0 ? NULL : LoadIcon(m_hInstance, MAKEINTRESOURCE(pInitInfo->IconID));
	wc.hIconSm       = pInitInfo->IconSmID == 0 ? NULL : LoadIcon(m_hInstance, MAKEINTRESOURCE(pInitInfo->IconSmID)); // Minimized window icon.
	wc.hCursor       = ManageCursor ? NULL : LoadCursor(NULL, IDC_ARROW); 
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);/*NULL;*/ // Background color.
	wc.lpszMenuName  = NULL;                   // Menu.
	wc.lpszClassName = pInitInfo->ClassName;            // Name of the window class.

	sprintf(&szCaption[0],   &pInitInfo->WindowCaption[0]);
	sprintf(&szClassName[0], &pInitInfo->ClassName[0]);
	
	// We must register the class in Windows.
	if(RegisterClassEx(&wc))
		Registered = true; 
	else
		return false;

	int ScrWidth  = GetSystemMetrics(SM_CXSCREEN);
	int ScrHeight = GetSystemMetrics(SM_CYSCREEN);
	if(pInitInfo->Width == 0 || pInitInfo->Height == 0){
		pInitInfo->Width  = ScrWidth;
		pInitInfo->Height = ScrHeight;
	}

	FullScreen = pInitInfo->FullScreen;
	WideScreen = pInitInfo->WideScreen;

    DWORD dwStyle;
	if(FullScreen){											
		// Set the window properties for full screen mode
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		
		// Change the display mode if needed
		if(pInitInfo->Width != ScrWidth || pInitInfo->Height != ScrHeight){
			
			DEVMODE DisplayMode;
			memset(&DisplayMode, 0, sizeof(DisplayMode));
			if(!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DisplayMode)){
				MessageBox(NULL, "Error: EnumDisplaySettings Failed ???", "Vortez3DEngine", 0);	
				return false;
			}

			DisplayMode.dmPelsWidth	 = pInitInfo->Width;
			DisplayMode.dmPelsHeight = pInitInfo->Height;
			//DisplayMode.dmBitsPerPel = 32;
			
			int ChangeDisplayResult = ChangeDisplaySettings(&DisplayMode, CDS_FULLSCREEN);	
			if(ChangeDisplayResult != DISP_CHANGE_SUCCESSFUL){
				MessageBox(NULL,"Error: Failed to change display mode.", "Vortez3DEngine", 0);
				return false;
			}
		}
		
		ClientRect.left = 0;	
		ClientRect.top	= 0;	
		ClientRect.right  = pInitInfo->Width; 
		ClientRect.bottom = pInitInfo->Height;
		
		memcpy(&WindowRect, &ClientRect, sizeof(RECT));
	} else {
		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		//dwStyle &= ~WS_THICKFRAME;

		DEVMODE DisplayMode;
		memset(&DisplayMode, 0, sizeof(DisplayMode));
		if(!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DisplayMode)){
			MessageBox(NULL,"Error: Failed to change display mode.", "Vortez3DEngine", 0);
			return false;
		}

		int ScrW = DisplayMode.dmPelsWidth;
		int ScrH = DisplayMode.dmPelsHeight;

		ClientRect.left = 0;	
		ClientRect.top	= 0;	

		float wRatio = (float)pInitInfo->Width  / (float)ScrW;
		float hRatio = (float)pInitInfo->Height / (float)ScrH;
		if(wRatio >= hRatio){
			if(wRatio > 1.0f){
				ClientRect.right  = (long)((float)pInitInfo->Width  / wRatio);
				ClientRect.bottom = (long)((float)pInitInfo->Height / wRatio);
			} else {
				ClientRect.right  = pInitInfo->Width; 
				ClientRect.bottom = pInitInfo->Height;
			}
		} else {
			if(hRatio > 1.0f){
				ClientRect.right  = (long)((float)pInitInfo->Width  / hRatio);
				ClientRect.bottom = (long)((float)pInitInfo->Height / hRatio);
			} else {
				ClientRect.right  = pInitInfo->Width; 
				ClientRect.bottom = pInitInfo->Height;
			}
		}
	
		memcpy(&WindowRect, &ClientRect, sizeof(RECT));
		
		AdjustWindowRectEx(&ClientRect, dwStyle, 0, 0x00000100);
	}
		
	// Create the window
	m_hWnd = CreateWindowEx(NULL,	// The extended window style
		GetClsName(),				// Window class
		GetCaption(),				// Window name
		dwStyle,					// Window style
		0,							// X coord
		0,							// Y coord
		GetWndWidth(),				// Height
		GetWndHeight(),				// Width
		NULL,						// Parent window handle
		NULL,						// Menu
		wc.hInstance,				// Handle to app. instance
		NULL);						// Unused
	
	// If there was an error with creating the window, then close the program
	if(!m_hWnd)
		return false;

	// Load the icons manually
	if(strlen(&pInitInfo->IconName[0]) > 0){
		HICON hIcon   = (HICON)LoadImage(NULL, pInitInfo->IconName,   IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
		if(hIcon)
			SendMessage(m_hWnd, WM_SETICON, ICON_BIG,   (LPARAM)hIcon);
	}
	if(strlen(&pInitInfo->IconSmName[0]) > 0){
		HICON hIconSm = (HICON)LoadImage(NULL, pInitInfo->IconSmName, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		if(hIconSm)
			SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);
	}

	// Show the window.and update its display
	switch(pInitInfo->StartMaximized)
	{
	case FALSE: ShowWindow(m_hWnd, SW_SHOW);     break;
	case TRUE:  ShowWindow(m_hWnd, SW_MAXIMIZE); break;
	}
	UpdateWindow(m_hWnd);

	//Confine the cursor to our window
	if(FullScreen)
		ClipCursor(&WindowRect);

	// Sets keyboard focus to the window
	SetFocus(m_hWnd);			 	

    return true;
}

void CGameWindow::Destroy()
{
	if(IsWindowAttached){
		m_hWnd = NULL;
		IsWindowAttached = false;
		return;
	}

	// Destroy the window
	if(m_hWnd){
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}

	// Unregister class
	if(Registered){
		if(UnregisterClass(&szClassName[0], m_hInstance)){
			m_hInstance = NULL;
			Registered  = false;
		}
	}
}

void CGameWindow::DisableResize(bool b)
{
	if(!FullScreen)
		CanResizeScreen = !b;
}

void CGameWindow::DisableMove(bool b)
{
	if(!FullScreen)
		CanMoveScreen = !b;	
}

void CGameWindow::ResizeToDesktopDimention()
{
	if(FullScreen || !m_hWnd)
		return;

	UpdateRects();

	RECT DeskTopRect;
	HWND h = FindWindowEx(0, 0, "Progman", "Program Manager");
	if(!h){return;}
	h = FindWindowEx(h, 0, "SHELLDLL_DefView", 0);
	if(!h){return;}
	h = FindWindowEx(h, 0, "SysListView32", "FolderView");
	if(!h){return;}
	GetClientRect(h, &DeskTopRect); 

	int DeskTopW = DeskTopRect.right  - DeskTopRect.left;
	int DeskTopH = DeskTopRect.bottom - DeskTopRect.top;

	int WndW = GetWndWidth();
	int WndH = GetWndHeight();

	int DifW = WndW - DeskTopW;
	int DifH = WndH - DeskTopH;

	DWORD NewWidth  = GetWndWidth();
	DWORD NewHeight = GetWndHeight();

	BOOL HaveChanged = FALSE;
	if(WndW > DeskTopW){
		NewWidth -= DifW;
		HaveChanged = TRUE;
	}
	if(WndH > DeskTopH){
		NewHeight -= DifH;
		HaveChanged = TRUE;
	}

	if(HaveChanged)
		MoveWindow(m_hWnd, 0,0, NewWidth, NewHeight, 1); 
}

void CGameWindow::UpdateRects()
{
	GetWindowRect(m_hWnd, &WindowRect);
	GetClientRect(m_hWnd, &ClientRect);
}

char* CGameWindow::GetCaption()
{
	GetWindowText(m_hWnd, &szCaption[0], 256);
	return &szCaption[0];
}

char* CGameWindow::GetClsName()
{
	GetClassName(m_hWnd, &szClassName[0], 256);
	return &szClassName[0];
}

void CGameWindow::SetDevSettings(int w, int h)
{
	DevSettings.Width  = w;
	DevSettings.Height = h;

	DevSettings.fWidth  = (float)w;
	DevSettings.fHeight = (float)h;
}


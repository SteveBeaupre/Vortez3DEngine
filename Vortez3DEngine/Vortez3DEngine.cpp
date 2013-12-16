#include "Vortez3DEngine.h"

CVortez3DEngine *pVortez3DEngine = NULL;
bool *pKeyState = NULL;
bool *pMouseBtnState = NULL;
__int64 *pPolygonsDrawed = NULL;
CGameWindow *pGameWindow = NULL;

#ifdef COMPILE_FOR_DIRECTX9
LPDIRECT3DDEVICE9 dxdev = NULL;
#else
void *pvVBO = NULL;
PFNGLACTIVETEXTUREARBPROC	    ActiveTextureARB = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC ClientActiveTextureARB = NULL;
PFNGLFOGCOORDPOINTEREXTPROC     FogCoordPointer = NULL;
#endif

//-----------------------------------------------------------------------------
// Application entry point
//-----------------------------------------------------------------------------
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwreason,  LPVOID lpReserved )
{
    switch(fdwreason)
	{
    case DLL_PROCESS_ATTACH: break;
    case DLL_THREAD_ATTACH:  break;
    case DLL_THREAD_DETACH:  break;
    case DLL_PROCESS_DETACH: break;
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// Constructor...
//-----------------------------------------------------------------------------
CVortez3DEngine::CVortez3DEngine()
{
	pVortez3DEngine = this;
	hInstance  = NULL;

	#ifdef COMPILE_FOR_DIRECTX9
		d3d        = NULL;
		d3ddev     = NULL;
		d3dFPSFont = NULL;
	#else
		hDC        = NULL;
		hRC        = NULL;
		hLoadingDC = NULL;
		hLoadingRC = NULL;
	#endif

	IsEngineInitialized = FALSE;
	#ifdef COMPILE_FOR_OPENGL
		IsOpenGLForLoadingInitialized = FALSE;
	#endif

	MouseClamped = false;
	pKeyState = &KeyState[0];
	pMouseBtnState = &MouseBtnState[0];
	ZeroMemory(&KeyState[0], 256);
	ZeroMemory(&MouseBtnState[0], 3);

	NumPolygonsDrawed = 0;
	pPolygonsDrawed = &NumPolygonsDrawed;
	
	pGameWindow = &GameWindow;

	#ifdef COMPILE_FOR_OPENGL
		pvVBO = &VBO;
		VBO.glGenBuffersARB = NULL;
		VBO.glGenBuffersARB = NULL;
		VBO.glGenBuffersARB = NULL;
		VBO.glGenBuffersARB = NULL;

		SwapIntervalEXT        = NULL;
		ActiveTextureARB	   = NULL;
		FogCoordPointer        = NULL;
		ClientActiveTextureARB = NULL;		

		NumTexelUnits = 0;
		MultitextureExtSupported = false;
		ShaderSupported = false;
		FogExtSupported = false;
	#endif

	Camera.ClearAll();
	Camera.SetDistance(-5000.0f);
	Camera.SetRotation(0.0f, 0.0f);
	Camera.SetTranslation(0.0f, 0.0f, 0.0f);
}

//-----------------------------------------------------------------------------
// Destructor...
//-----------------------------------------------------------------------------
CVortez3DEngine::~CVortez3DEngine()
{
	ShutdownEngine();
}

#ifdef COMPILE_FOR_OPENGL
bool CVortez3DEngine::IsVBOSupported()
{
	CVBO *pVBO = (CVBO*)pvVBO;
	return pVBO->IsVBOSupported;
}

CVBO* CVortez3DEngine::GetVBOPtr()
{
	return (CVBO*)pvVBO;
}
#endif

//-----------------------------------------------------------------------------
// Initialise the engine...
//-----------------------------------------------------------------------------
bool CVortez3DEngine::InitEngine(CInitInfo *pInitInfo)
{
	if(IsEngineInitialized)
		return false;
	
	//Get the application HINSTANCE
	hInstance = pInitInfo->hInst;

	if(pInitInfo->wndhandle == NULL){
		// Init. those 2 strings if they are empty
		if(!pInitInfo->ClassName){sprintf(pInitInfo->ClassName, "Vortez3DEngine Class");}
		if(!pInitInfo->WindowCaption){sprintf(pInitInfo->WindowCaption, "Vortez3DEngine - Created by Vortez");}
	}

	//Create the game windows
	if(!GameWindow.Create(pInitInfo, &MsgProc)){
		MessageBox(0, "Unable to create main windows...", "Error!", 0);
		goto EndFunc;
	}

	//Init. OpenGL or DirectX
	#ifdef COMPILE_FOR_DIRECTX9
		if(!InitializeDirectX9()){
			MessageBox(0, "Unable to start DirectX...", "Error!", 0);
			goto EndFunc;
		}
	#else
		if(!InitializeOpenGL()){
			MessageBox(0, "Unable to startup OpenGL...", "Error!", 0);
			goto EndFunc;
		}
	#endif
	
	if(pInitInfo->wndhandle == NULL){
		// Adjust the game window to the desktop size, for windowed mode only...
		GameWindow.ResizeToDesktopDimention();
	}

	// Init. the FPS Font
	#ifdef COMPILE_FOR_DIRECTX9
		D3DXCreateFont(d3ddev, 16, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &d3dFPSFont);
	#else
		#ifdef USE_BMP_FPS
		FPSFont.Build(hDC, "Arial", 12, 0, 0, 255, 255, 0);
		#else
		FPSFont.Load("Arial", 12, BLEND_FONT_NORMAL, FONT_FILTER_LINEAR);
		#endif
	#endif

	// Init. the VBO
	#ifdef COMPILE_FOR_OPENGL
		#ifndef NO_VBO
			VBO.IsVBOSupported = GLCompatibility.CheckExtension("GL_ARB_vertex_buffer_object");
		#else
			VBO.IsVBOSupported = false; // A recoder
		#endif
		VBO.InitVBOPointers();
	#endif

	// Allow custom initialisation code...
	InitEngineExt();

	// Make sure we clean this up if we forget about it
	#ifdef COMPILE_FOR_OPENGL
		CleanUpOpenGLForLoading();
	#endif

	//Reset the timer...
	EngineTimer.Reset();
	EngineTimer.Tick();

	// Tell the engine that initialization is done
	IsEngineInitialized = TRUE;

	EndFunc:
	return IsEngineInitialized == TRUE;
}

//-----------------------------------------------------------------------------
// Start the "game loop"
//-----------------------------------------------------------------------------
void CVortez3DEngine::StartEngine()
{
	GameLoop();
}

//-----------------------------------------------------------------------------
// Shutdown the engine...
//-----------------------------------------------------------------------------
void CVortez3DEngine::ShutdownEngine()
{
	// Make sure the engine is initialized
	if(IsEngineInitialized){

		// Allow custom shutdown code...
		ShutdownEngineExt();

		#ifdef COMPILE_FOR_DIRECTX9
			// Free the FPS font
			SAFE_RELEASE(d3dFPSFont);
			// Shutdown DirectX
			ShutdownDirectX9();
		#else
			// Free the FPS font
			FPSFont.Free();
			// Shutdown OpenGL
			ShutdownOpenGL();
		#endif
		
		// Destroy the game window
		GameWindow.Destroy();

		// Tell the engine that everything has been uninitialized
		IsEngineInitialized = FALSE;

		// Show memory leaks
		#ifdef LEAK_TRACKER_PRESENT
			LeakTrackerList.DumpUnfreed();
		#endif
	}
}

void CTexturesManagerExt::OnTextureDeleted(UINT TexID)
{
	pVortez3DEngine->ModelsManager.RemoveTextureID(TexID);
	pVortez3DEngine->SpritesManager.RemoveTextureID(TexID);
}

//-----------------------------------------------------------------------------
// Process messages not handled by the messages pump
//-----------------------------------------------------------------------------
void OnCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	// Internal winsock messages
	case ON_WINSOCK_MESSAGE: 
		pVortez3DEngine->OnWinsockMsg(uMsg, wParam, lParam); 
		break;
	// External network messages
	case ON_NETWORK_MESSAGE: 
		pVortez3DEngine->OnNetworkMsg(uMsg, wParam, lParam); 
		break;

	// Sound and music messages
	case WM_PLAY_SOUNDFX: pVortez3DEngine->PlaySoundFXExt((int)lParam); break;
	case WM_PLAY_TUNE   : pVortez3DEngine->PlayTuneExt((int)lParam);    break;

	// Call OnCustomMessageExt to process custom messages
	default: pVortez3DEngine->OnCustomMessageExt(uMsg, wParam, lParam); break;
	}
}

//-----------------------------------------------------------------------------
// Disable window sizing or moving if CanResize or CanMove are set to true
//-----------------------------------------------------------------------------
LRESULT HitTest(int hitLocation, HWND hWnd)
{
	BOOL IsFullScreen = pVortez3DEngine->GameWindow.IsFullScreen();
	BOOL CanResize = pVortez3DEngine->GameWindow.CanResize();
	BOOL CanMove = pVortez3DEngine->GameWindow.CanMove();

	// Are we in fullscreen mode?
	if(IsFullScreen)
		return hitLocation;

	switch(hitLocation)
	{
	case HTCLIENT:      
		return hitLocation; 
	case HTCAPTION:
		{
			// If the windows is maximized, they're no need to disable it
			WINDOWPLACEMENT wp;
			GetWindowPlacement(hWnd, &wp);

			if(CanMove || wp.showCmd == SW_SHOWMAXIMIZED)
				return hitLocation;
		}
		break;
	case HTLEFT:        
	case HTRIGHT:        
	case HTTOP:         
	case HTBOTTOM:      
	case HTTOPLEFT:     
	case HTTOPRIGHT:    
	case HTBOTTOMLEFT:  
	case HTBOTTOMRIGHT: 
	case HTSIZE:        
		if(CanResize)
			return hitLocation; 
		break;
	case HTMINBUTTON:   
	case HTMAXBUTTON:   
	case HTCLOSE:       
	default:
		return hitLocation; 
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Process the windows messages
//-----------------------------------------------------------------------------
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	//Process quit message
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		break;	

	//Screensaver messages
	case WM_SYSCOMMAND:			// Intercept System Commands
		switch (wParam)			// Check System Calls
		{
		case SC_SCREENSAVE:		// Screensaver Trying To Start?
		case SC_MONITORPOWER:	// Monitor Trying To Enter Powersave?
			return 0;			// Prevent From Happening
		}
		break;		
	
	// Ignore this...
	case WM_STYLECHANGED:
		return 0;

	// Non-Client area messages
	case WM_NCHITTEST:
		return HitTest(DefWindowProc(hWnd, uMsg, wParam, lParam), hWnd);

	// Resize message
	case WM_SIZE:
		pVortez3DEngine->GameWindow.UpdateRects();
		pVortez3DEngine->OnGameWindowResized(LOWORD(lParam), HIWORD(lParam));
		break;

	// Keyboard messages, for text...
	case WM_CHAR:
		pVortez3DEngine->OnChar((UINT)wParam);
		break;	

	// Keyboard messages, , for keys...
	case WM_KEYDOWN:
		if(wParam == VK_ESCAPE && pVortez3DEngine->GetUseEscKeyToQuit()){
			PostQuitMessage(0);
			break;
		}
		pKeyState[wParam] = true;
		pVortez3DEngine->OnKeyDown((UINT)wParam);
		break;	
	case WM_KEYUP:
		pKeyState[wParam] = false;
		pVortez3DEngine->OnKeyUp((UINT)wParam);
		break;

	// Mouse move messages
	case WM_MOUSEMOVE:
		pVortez3DEngine->OnMouseMove(LOWORD(lParam), HIWORD(lParam));
		break;
	
	// Mouse buttons messages
	case WM_LBUTTONDOWN:
		pMouseBtnState[LEFT_MOUSE_BUTTON] = true;
		pVortez3DEngine->OnMouseDown(LEFT_MOUSE_BUTTON, (int)LOWORD(lParam), (int)HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
		pMouseBtnState[LEFT_MOUSE_BUTTON] = false;
		pVortez3DEngine->OnMouseUp(LEFT_MOUSE_BUTTON, (int)LOWORD(lParam), (int)HIWORD(lParam));
		break;
	case WM_MBUTTONDOWN:
		pMouseBtnState[MIDDLE_MOUSE_BUTTON] = true;
		pVortez3DEngine->OnMouseDown(MIDDLE_MOUSE_BUTTON, (int)LOWORD(lParam), (int)HIWORD(lParam));
		break;
	case WM_MBUTTONUP:
		pMouseBtnState[MIDDLE_MOUSE_BUTTON] = false;
		pVortez3DEngine->OnMouseUp(MIDDLE_MOUSE_BUTTON, (int)LOWORD(lParam), (int)HIWORD(lParam));
		break;
	case WM_RBUTTONDOWN:
		pMouseBtnState[RIGHT_MOUSE_BUTTON] = true;
		pVortez3DEngine->OnMouseDown(RIGHT_MOUSE_BUTTON, (int)LOWORD(lParam), (int)HIWORD(lParam));
		break;
	case WM_RBUTTONUP:
		pMouseBtnState[RIGHT_MOUSE_BUTTON] = false;
		pVortez3DEngine->OnMouseUp(RIGHT_MOUSE_BUTTON, (int)LOWORD(lParam), (int)HIWORD(lParam));
		break;

	//Mouse wheel message
	case WM_MOUSEWHEEL:
		{
			int RollCount = (signed short)HIWORD(wParam) / WHEEL_DELTA;
			pVortez3DEngine->OnMouseRoll(RollCount);
		}
		break;

	// Process other messages
	default: 
		OnCustomMessage(uMsg, wParam, lParam);
		break;
	}

	//Return result to windows
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Empty the messages queue so all pending messages will be processed...
//-----------------------------------------------------------------------------
void CVortez3DEngine::ProcessMessages()
{
	MSG msg;
	// Application loop.
	while(PeekMessage(&msg, GethWnd(), NULL, NULL, PM_REMOVE)){
		TranslateMessage(&msg);  // Translate any messages.
		DispatchMessage(&msg);   // Dispatch any messages.
	}
}

//-----------------------------------------------------------------------------
// The game loop...
//-----------------------------------------------------------------------------
void CVortez3DEngine::GameLoop()
{
	MSG msg;
	bool done = false;

	// Empty the nessages queue if this isn't the first loop
	static bool IsFirstLoop = true;
	if(!IsFirstLoop)
		while(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE));

	// Application loop.
	while(!done){
		if(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)){
			
			// Disable F10 key
			if(msg.message == WM_SYSKEYDOWN && msg.wParam == VK_F10)
				continue;

			// If a quit message is received then stop the loop
			if(msg.message == WM_QUIT)
				done = true;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			// Reset the polygon rendered counter
			NumPolygonsDrawed = 0;
			// Update the scene and render a frame
			UpdateScene(EngineTimer.Tick());
    		RenderScene();
		}
	}

	// Tell the application that we've run the game loop before
	IsFirstLoop = false;
}


//-----------------------------------------------------------------------------
// Process mouse move messages
//-----------------------------------------------------------------------------
void CVortez3DEngine::OnMouseMove(int x, int y)
{
	static int Last_X = x;
	static int Last_Y = y;

	// Additionnal mouse move code 
	OnMouseMove(x, y, x - Last_X, y - Last_Y);

	Last_X = x;
	Last_Y = y;
}

#ifdef COMPILE_FOR_OPENGL
//-----------------------------------------------------------------------------
// Setup OpenGL rendering context
//-----------------------------------------------------------------------------
bool CVortez3DEngine::SetupPixelFormatDescriptor(HDC hdc)
{ 
    int pixelformat = 0; 
	DWORD Flags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;

    PIXELFORMATDESCRIPTOR pfd; 
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));    // Erase the structure
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);			// Set the size of the structure
    pfd.nVersion = 1;									// Always set this to 1
    pfd.dwFlags = Flags;                                // Pass in the appropriate OpenGL flags
    pfd.dwLayerMask = PFD_MAIN_PLANE;					// We want the standard mask (this is ignored anyway)
    pfd.iPixelType = PFD_TYPE_RGBA;						// We want RGBA pixel type
    pfd.cColorBits = 32;								// How many color bit?
    pfd.cDepthBits = 24;								// Depthbits is ignored for RGBA, but we do it anyway
    pfd.cStencilBits = 8;								// We'll use stencil buffer
    pfd.cAlphaBits = 8;	                                // 1 Alpha byte
    pfd.cAccumBits = 0;									// No special bitplanes needed
	
	// This gets us a pixel format that best matches the one passed in from the device
    if((pixelformat = ChoosePixelFormat(hdc, &pfd)) == false){ 
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
	// This sets the pixel format that we extracted from above
    if(SetPixelFormat(hdc, pixelformat, &pfd) == false){ 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    return true;   // Return a success!
}

#ifdef COMPILE_FOR_OPENGL_3_1_
//-----------------------------------------------------------------------------
// Initialize shaders data
//-----------------------------------------------------------------------------
void CShaderExt::InitShaderDataExt()
{
	LightLoc = glGetUniformLocation(m_Program, "LightPosition");
	glUniform3f(LightLoc, 0.0, 0.0, 4.0);
	TexLoc   = glGetUniformLocation(m_Program, "TexLoc");
	glUniform1i(TexLoc, 0);
}
#endif

//-----------------------------------------------------------------------------
// Initialize OpenGL
//-----------------------------------------------------------------------------
bool CVortez3DEngine::InitializeOpenGL() 
{  
    hDC = GetDC(GameWindow.GetHWND());					// This sets our global HDC
														// We don't free this hdc until the end of our program
    if(!SetupPixelFormatDescriptor(hDC)){				// This sets our pixel format/information
		PostQuitMessage(0);								// If there's an error, quit
		return false;
	}

	#ifndef COMPILE_FOR_OPENGL_3_1_
    hRC = wglCreateContext(hDC);		   				// This creates a rendering context from our hdc
    wglMakeCurrent(hDC, hRC);							// This makes the rendering context we just created the one we want to use
	#else
	GLenum err = glewInit();
    HGLRC tempContext = wglCreateContext(hDC);		   	// This creates a rendering context from our hdc
    wglMakeCurrent(hDC, tempContext);					// This makes the rendering context we just created the one we want to use
	int attribs[] =
	{
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 1,
        WGL_CONTEXT_FLAGS_ARB, 0,
        0
	};
	PFNWGLCREATECONTEXTATTRIBSARBPROC CreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	hRC = CreateContextAttribsARB(hDC, 0, attribs);
	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(tempContext);
	wglMakeCurrent(hDC, hRC);
	#endif

	//Gamma.UpdateGamma(&m_hDC);                        // Gamma correction

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Black Background
	glColor3f(1.0f, 1.0f, 1.0f);				        // Current Color
	
	#ifdef COMPILE_FOR_OPENGL_3_1_
		if(!DefaultShader.LoadShaders("Shaders\\minimal.vert", /*NULL*/"Shaders\\minimal.frag")){
			MessageBox(NULL, "Unable to load shaders.", "Error", MB_OK);
			return false;
		}
	#endif

	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glEnable(GL_LINE_SMOOTH);							// Enables Depth Testing
	glShadeModel(GL_SMOOTH);                            // Enable Smooth Shading
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);			// Really Nice Perspective Calculations
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);				// Really Nice Perspective Calculations
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);			// Really Nice Perspective Calculations

	glEnable(GL_LIGHTING);			                    // Turn on lighting
	glEnable(GL_LIGHT0);			                    // Turn on a light with defaults set
	GLfloat light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light_position[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_COLOR_MATERIAL);	                    // Allow color

	if(MultitextureExtSupported = GLCompatibility.CheckExtension("GL_ARB_multitexture")){	
		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &NumTexelUnits);
		ActiveTextureARB	   = (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
		ClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)	wglGetProcAddress("glClientActiveTextureARB");		
	}

	if(GLCompatibility.CheckVersion(1,4) || GLCompatibility.CheckExtension("EXT_fog_coord")){
		FogExtSupported = true;
		FogCoordPointer = (PFNGLFOGCOORDPOINTEREXTPROC)wglGetProcAddress("glFogCoordPointer");
	}

	if(GLCompatibility.CheckExtension("WGL_EXT_swap_control"))
		SwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");

	if(GLCompatibility.CheckExtension("GL_ARB_vertex_program") && 
	   GLCompatibility.CheckExtension("GL_ARB_vertex_shader") && 
	   GLCompatibility.CheckExtension("GL_ARB_fragment_program") && 
	   GLCompatibility.CheckExtension("GL_ARB_fragment_shader")){
		ShaderSupported = true;
	}

	return true;
}


//-----------------------------------------------------------------------------
// Shutdown OpenGL
//-----------------------------------------------------------------------------
void CVortez3DEngine::ShutdownOpenGL()
{
	#ifdef COMPILE_FOR_OPENGL_3_1_
	DefaultShader.DisableShader();
	DefaultShader.FreeShader();
	//#else
	#endif

	// Delete the rendering context
	if(hRC){											
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hRC);
		hRC = NULL;
	}

	// Release the hDC
	if(hDC){
		ReleaseDC(GameWindow.GetHWND(), hDC);
		hDC = NULL;
	}
	
	// Revert desktop resolution changes, if any...
	if(GameWindow.IsFullScreen())
		ChangeDisplaySettings(NULL,0);
}
#endif

#ifdef COMPILE_FOR_DIRECTX9
//-----------------------------------------------------------------------------
// Initialize DirectX
//-----------------------------------------------------------------------------
bool CVortez3DEngine::InitializeDirectX9()
{
	HWND hWnd = GethWnd();

	// Create the Direct3D interface
	d3d = Direct3DCreate9(D3D_SDK_VERSION);    

	bool IsHardwareVertexProcessingSupported = false;
	D3DCAPS9 caps;
	d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		IsHardwareVertexProcessingSupported = true;
	DWORD Behavior = IsHardwareVertexProcessingSupported ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    D3DPRESENT_PARAMETERS d3dpp;    
    ZeroMemory(&d3dpp, sizeof(d3dpp));    
	d3dpp.BackBufferWidth  = 1024;
	d3dpp.BackBufferHeight = 768;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;   
	d3dpp.Windowed = TRUE;    
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferCount = 1;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    d3dpp.hDeviceWindow = hWnd;

	// Create a device class using this information and information from the d3dpp stuct
    HRESULT Res = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, Behavior, &d3dpp, &d3ddev);

	// Is everything ok?
	if(Res == D3D_OK){

		// Set the extern device pointer used by other modules
		dxdev = d3ddev;

		// Create a light
		D3DLIGHT9 light;
		ZeroMemory(&light, sizeof(light));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Direction = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		// Turn on light #0
		d3ddev->SetLight(0, &light);
		d3ddev->LightEnable(0, TRUE);  
 		d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE);
		d3ddev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255));

		// Enable back buffer
		d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);
	}

	return Res == D3D_OK;
}

//-----------------------------------------------------------------------------
// Shutdown DirectX
//-----------------------------------------------------------------------------
void CVortez3DEngine::ShutdownDirectX9()
{
	// Close and release the 3D device
	SAFE_RELEASE(d3ddev);
	// Close and release Direct3D
	SAFE_RELEASE(d3d); 
	// Reset the extern device pointer
	dxdev = NULL;
}
#endif

#ifdef COMPILE_FOR_OPENGL
//-----------------------------------------------------------------------------
// Initialize a second OpenGL rendering context, for load screen purpose
//-----------------------------------------------------------------------------
bool CVortez3DEngine::InitializeOpenGLForLoading()
{
	if(IsOpenGLForLoadingInitialized)
		return false;

	hLoadingDC = GetDC(GethWnd());

	PIXELFORMATDESCRIPTOR pfd; 
	int pixelformat; 

	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);			// Set the size of the structure
	pfd.nVersion = 1;									// Always set this to 1
														// Pass in the appropriate OpenGL flags
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
	pfd.dwLayerMask = PFD_MAIN_PLANE;					// We want the standard mask (this is ignored anyway)
	pfd.iPixelType = PFD_TYPE_RGBA;						// We want RGBA pixel type
	pfd.cColorBits = 32;								// How many color bit?
	pfd.cDepthBits = 24;								// Depthbits is ignored for RGBA, but we do it anyway
	pfd.cAccumBits = 0;									// No special bitplanes needed
	pfd.cStencilBits = 8;								// We'll use stencil buffer
	pfd.cAlphaBits = 8;	                                // 1 Alpha byte
	
	// This gets us a pixel format that best matches the one passed in from the device
	if((pixelformat = ChoosePixelFormat(hLoadingDC, &pfd)) == false){ 
		MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
		PostQuitMessage(0);	// If there's an error, quit
		return false;
	} 

	// This sets the pixel format that we extracted from above
	if(SetPixelFormat(hLoadingDC, pixelformat, &pfd) == false){ 
		MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
		PostQuitMessage(0);	// If there's an error, quit
		return false;
	} 
	
	////////////////////////////////////////////////////////////////////////////////

	// This creates a rendering context from our hdc
    hLoadingRC = wglCreateContext(hLoadingDC);

	// This makes the rendering context we just created the one we want to use
    wglMakeCurrent(hLoadingDC, hLoadingRC);	

	//Gamma.UpdateGamma(&g_hDC);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Black Background
	
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glEnable(GL_LINE_SMOOTH);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glShadeModel(GL_SMOOTH);                            // Enable Smooth Shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);			// Really Nice Perspective Calculations
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);				// Really Nice Perspective Calculations
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);			// Really Nice Perspective Calculations

	glEnable(GL_LIGHTING);			                    // Turn on lighting
	glEnable(GL_LIGHT0);			                    // Turn on a light with defaults set

	IsOpenGLForLoadingInitialized = true;
	OnInitializeOpenGLForLoadingExt();

	return true;
}

//-----------------------------------------------------------------------------
// Cleanup the second OpenGL rendering context, for load screen purpose
//-----------------------------------------------------------------------------
void CVortez3DEngine::CleanUpOpenGLForLoading()
{
	if(!IsOpenGLForLoadingInitialized)
		return;

	//Cleanup the load screen system
	wglMakeCurrent(hLoadingDC, hLoadingRC);
	OnCleanUpOpenGLForLoadingExt();

	// Delete the rendering context
	if(hLoadingRC){											
		wglMakeCurrent(NULL, NULL);	
		wglDeleteContext(hLoadingRC);
	}
	
	// Release the hDC
	if(hLoadingDC) 
		ReleaseDC(GethWnd(), hLoadingDC);

	// Select the main rendering context
	wglMakeCurrent(GethDC(), GethGLRC());

	IsOpenGLForLoadingInitialized = false;
}

//-----------------------------------------------------------------------------
// The load screen render code
//-----------------------------------------------------------------------------
void CVortez3DEngine::RenderLoading(int PercentDone)
{
	if(!IsOpenGLForLoadingInitialized)
		return;

	//Pick the Loading rendering context
    wglMakeCurrent(hLoadingDC, hLoadingRC);

	//Clear The Color, depth and stencil buffer
	glClear(GL_COLOR_BUFFER_BIT);	
	glColor3f(1.0f, 1.0f, 1.0f);

	// Call the virtual function and check if we need to swap the buffer
	RenderLoadingExt(PercentDone);

	//Swap buffers
	SwapBuffers(hLoadingDC);

	//Pick the normal RC
    wglMakeCurrent(GethDC(), GethGLRC());
	
	// Process waiting messages
	ProcessMessages();
}

//-----------------------------------------------------------------------------
// Return the current TU
//-----------------------------------------------------------------------------
UINT CVortez3DEngine::GetCurrentTextureUnit()
{
	int CurrentActiveTextureUnit = 0;
	glGetIntegerv(GL_ACTIVE_TEXTURE_ARB, &CurrentActiveTextureUnit);
	return CurrentActiveTextureUnit - GL_TEXTURE0_ARB;
}

//-----------------------------------------------------------------------------
// Set the current TU
//-----------------------------------------------------------------------------
void CVortez3DEngine::SetCurrentTextureUnit(UINT Val)
{
	if(ActiveTextureARB && ClientActiveTextureARB){
		ActiveTextureARB(GL_TEXTURE0_ARB + Val);
		ClientActiveTextureARB(GL_TEXTURE0_ARB + Val);
	}
}
#endif

//-----------------------------------------------------------------------------
// Draw the FPS in the top-left corner
//-----------------------------------------------------------------------------
void CVortez3DEngine::DrawFPS()
{
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, &vp[0]);
	DrawFPS(false, &vp[0]);
}

//-----------------------------------------------------------------------------
// Draw the FPS in the top-left corner
//-----------------------------------------------------------------------------
void CVortez3DEngine::DrawFPS(bool Rescale, int *pVPCoords, float r, float g, float b)
{
	#ifdef COMPILE_FOR_DIRECTX9
		RECT Rect;
		Rect.left    = 15;
		Rect.top     = Rect.left;
		Rect.right   = Rect.left + 200;
		Rect.bottom  = Rect.left + 25;
		char FPSBuf[32];
		ZeroMemory(&FPSBuf[0], 32);
		sprintf(&FPSBuf[0], "FPS: %2.2f", EngineTimer.GetFPS());
		d3dFPSFont->DrawText(NULL, &FPSBuf[0], -1, &Rect, DT_TOP | DT_LEFT, D3DCOLOR_XRGB((BYTE)(r * 255.0f), (BYTE)(g * 255.0f), (BYTE)(b * 255.0f)));
	#else
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		Camera.Set2DMode(pVPCoords);

		#ifdef USE_BMP_FPS
		FPSFont.SetColor((BYTE)(r * 255.0f),(BYTE)(g * 255.0f),(BYTE)(b * 255.0f));
		FPSFont.glPrintArg(10, GameWindow.GetHeight()-25, "FPS: %2.2f", EngineTimer.GetFPS());
		#else
		FPSFont.PrintArg(0, 0, Rescale, 15.0f, 15.0f, GameWindow.GetWidthDevRatio(), GameWindow.GetHeightDevRatio(), r, g, b, "FPS: %2.2f", EngineTimer.GetFPS());
		#endif
	#endif
}

//-----------------------------------------------------------------------------
// Usefull function for animations
//-----------------------------------------------------------------------------
inline void CVortez3DEngine::TimeStuff(float *var, float ElapsedTime, int TimeScale, float ElpasedTimeRatio, float Limit)
{
	*var += (ElapsedTime * ElpasedTimeRatio) * (float)TimeScale; 
	if(*var >= Limit){
		while(*var >= Limit)
			*var -= Limit;
		if(*var <= 0.0f)
			*var = 0.0f;
	}
}

#ifdef COMPILE_FOR_DIRECTX9
//-----------------------------------------------------------------------------
// Rectangle drawing function
//-----------------------------------------------------------------------------
void CVortez3DEngine::DrawQuadsDX(float Left, float Top, float Width, float Height)
{
}

#else
//-----------------------------------------------------------------------------
// Rectangle drawing function
//-----------------------------------------------------------------------------
void CVortez3DEngine::DrawQuads(float Left, float Top, float Width, float Height, float4 *pTexCoords, bool Rescale, bool HCentered, bool VCentered)
{
	float l,r,t,b;
	float TexL,TexW,TexT,TexH;

	if(Rescale){
		float w = GameWindow.GetWidthf();
		float h = GameWindow.GetHeightf();

		// Make the quad centered verticaly
		if(VCentered)
			Left = (w - (Width * w)) / 2.0f;
		// Make the quad centered horizontaly
		if(HCentered)
			Top = (h - (Height * h)) / 2.0f;

		// Calculate our points
		l = Left * w;
		r = (Left + Width) * w;
		t = h - (Top * h);
		b = h - ((Top + Height) * h);
	} else {
		float w = GameWindow.GetDevWidthf();
		float h = GameWindow.GetDevHeightf();

		// Make the quad centered verticaly
		if(VCentered)
			Left = (w - Width) / 2.0f;
		// Make the quad centered horizontaly
		if(HCentered)
			Top = (h - Height) / 2.0f;

		// Calculate our points
		l = Left;
		r = Left + Width;
		t = h - Top;
		b = h - (Top + Height);
	}

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

	// Draw the quad...
	glBegin(GL_QUADS);
		glTexCoord2f(TexL,        TexT + TexH); glVertex2f(l, t);
		glTexCoord2f(TexL + TexW, TexT + TexH); glVertex2f(r, t);
		glTexCoord2f(TexL + TexW, TexT);        glVertex2f(r, b);
		glTexCoord2f(TexL,        TexT); 		glVertex2f(l, b);
	glEnd();
}	

//-----------------------------------------------------------------------------
// Rectangle drawing function
//-----------------------------------------------------------------------------
void CVortez3DEngine::DrawQuads(CQuadRect *pCoords, float4 *pTexCoords, bool Rescale, bool HCentered, bool VCentered)
{
	switch(Rescale)
	{
	case false: DrawQuads(pCoords->L, pCoords->T, pCoords->W, pCoords->H, pTexCoords, Rescale, HCentered, VCentered); break;
	case true:  DrawQuads(pCoords->L / GameWindow.GetDevWidthf(), pCoords->T / GameWindow.GetDevHeightf(), pCoords->W / GameWindow.GetDevWidthf(), pCoords->H / GameWindow.GetDevHeightf(), pTexCoords, Rescale, HCentered, VCentered); break;
	}
}

//-----------------------------------------------------------------------------
// Rectangle drawing function, with texture
//-----------------------------------------------------------------------------
void CVortez3DEngine::DrawTexQuads(UINT TexID, float Left, float Top, float Width, float Height, float4 *pTexCoords, bool Rescale, bool HCentered, bool VCentered)
{
	// Set the texture id
	glBindTexture(GL_TEXTURE_2D, TexID);
	
	switch(Rescale)
	{
	case false: DrawQuads(Left * GameWindow.GetDevWidthf(), Top * GameWindow.GetDevHeightf(), Width * GameWindow.GetDevWidthf(), Height * GameWindow.GetDevHeightf(), pTexCoords, Rescale, HCentered, VCentered); break;
	case true:  DrawQuads(Left, Top, Width, Height, pTexCoords, Rescale, HCentered, VCentered); break;
	}
}

//-----------------------------------------------------------------------------
// Rectangle drawing function, with texture
//-----------------------------------------------------------------------------
void CVortez3DEngine::DrawTexQuads(UINT TexID, CQuadRect *pCoords, float4 *pTexCoords, bool Rescale, bool HCentered, bool VCentered)
{
	// Set the texture id
	glBindTexture(GL_TEXTURE_2D, TexID);
	switch(Rescale)
	{
	case false: DrawQuads(pCoords->L, pCoords->T, pCoords->W, pCoords->H, pTexCoords, Rescale, HCentered, VCentered); break;
	case true:  DrawQuads(pCoords->L / GameWindow.GetDevWidthf(), pCoords->T / GameWindow.GetDevHeightf(), pCoords->W / GameWindow.GetDevWidthf(), pCoords->H / GameWindow.GetDevHeightf(), pTexCoords, Rescale, HCentered, VCentered); break;
	}
}
#endif

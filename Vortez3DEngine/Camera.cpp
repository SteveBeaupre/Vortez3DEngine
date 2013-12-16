#include "Camera.h"

CCamera::CCamera()
{
	DisableLocks();
	ClearAll();
}

void CCamera::ClearAll()
{
	ZeroMemory(&Settings, sizeof(CCameraSettings));
	ZeroMemory(&SettingsBackup, sizeof(CCameraSettings));

	Settings.MinXRotLock = 0.0f;
	Settings.MaxXRotLock = 360.0f;
	Settings.MinYRotLock = -90.0f;
	Settings.MaxYRotLock = 90.0f;
}

void CCamera::SetDefaultValue(CCameraSettings *pSettings)
{
	Settings.NearPlane = pSettings->NearPlane;
	Settings.FarPlane  = pSettings->FarPlane;
	SetRotation(pSettings->xRotation, pSettings->yRotation);
	SetTranslation(pSettings->xTranslation, pSettings->yTranslation, pSettings->zTranslation);
	Settings.CameraDistance         = pSettings->CameraDistance;
	Settings.CameraRotationSpeed    = pSettings->CameraRotationSpeed;
	Settings.CameraTranslationSpeed = pSettings->CameraTranslationSpeed;
	Settings.CameraZoomSpeed        = pSettings->CameraZoomSpeed;
	SetLocks(pSettings->MinXRotLock, pSettings->MaxXRotLock, pSettings->MinYRotLock, pSettings->MaxYRotLock);
}

void CCamera::SetDefaultValue(float Near, float Far, float RotX, float RotY, float TransX, float TransY, float TransZ, float CamDist, float CamRotSpd, float CamTransSpd, float CamZoomSpd, float MinXLock, float MaxXLock, float MinYLock, float MaxYLock)
{
	Settings.NearPlane = Near;
	Settings.FarPlane  = Far;
	SetRotation(RotX, RotY);
	SetTranslation(TransX, TransY, TransZ);
	Settings.CameraDistance         = CamDist;
	Settings.CameraRotationSpeed    = CamRotSpd;
	Settings.CameraTranslationSpeed = CamTransSpd;
	Settings.CameraZoomSpeed        = CamZoomSpd;
	SetLocks(MinXLock, MaxXLock, MinYLock, MaxYLock);
}

////////////////////////////////////////////////////////////////////////////////////////////

void CCamera::SaveSettings()
{
	memcpy(&SettingsBackup, &Settings, sizeof(CCameraSettings));
}

////////////////////////////////////////////////////////////////////////////////////////////

void CCamera::RestoreSettings()
{
	memcpy(&Settings, &SettingsBackup, sizeof(CCameraSettings));
}

////////////////////////////////////////////////////////////////////////////////////////////

void CCamera::SetCamera()
{
	#ifdef COMPILE_FOR_DIRECTX9
		D3DXVECTOR3 CameraPosition = D3DXVECTOR3(0.0f, 0.0f, Settings.CameraDistance);
		D3DXVECTOR3 CameraTarget   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 CameraUpVector = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		D3DXMATRIX matView;
		D3DXMatrixIdentity(&matView);
		D3DXMatrixLookAtLH(&matView, &CameraPosition, &CameraTarget, &CameraUpVector);

		D3DXMATRIX matRotationX, matRotationY, matTranslation;
		D3DXMatrixRotationX(&matRotationX, D3DXToRadian(Settings.yRotation));
		D3DXMatrixRotationY(&matRotationY, D3DXToRadian(Settings.xRotation));
		D3DXMatrixTranslation(&matTranslation, Settings.xTranslation, Settings.yTranslation, Settings.zTranslation);

		//matView *= matTranslation * matRotationX * matRotationY;

		dxdev->SetTransform(D3DTS_VIEW, &(matTranslation*matRotationY*matRotationX*matView));
	#else
		gluLookAt(0.0f,0.0f,Settings.CameraDistance, 0.0f,0.0f,0.0f, 0.0f,1.0f,0.0f);
		glRotatef(Settings.yRotation, 1.0f, 0.0f, 0.0f);
		glRotatef(Settings.xRotation, 0.0f, 1.0f, 0.0f);
		glTranslatef(Settings.xTranslation, Settings.yTranslation, Settings.zTranslation);
	#endif
}

////////////////////////////////////////////////////////////////////////////////////////////

void CCamera::IncDistance(float CamDist)
{
	Settings.CameraDistance += CamDist * Settings.CameraZoomSpeed;
	/*if(Settings.CameraDistance >= 0){ 
		if(Settings.CameraDistance < Settings.CameraZoomSpeed)
			Settings.CameraDistance = Settings.CameraZoomSpeed;
	} else {
		if(Settings.CameraDistance > -Settings.CameraZoomSpeed)
			Settings.CameraDistance = -Settings.CameraZoomSpeed;
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////

void CCamera::ClampRotation(float *r)
{
	while(*r < 0.0f){*r += 360.0f;}
	while(*r >= 360.0f){*r -= 360.0f;}	
}

////////////////////////////////////////////////////////////////////////////////////////////

void CCamera::IncRotation(float xRot, float yRot)
{
	Settings.xRotation += xRot * Settings.CameraRotationSpeed; 
	Settings.yRotation += yRot * Settings.CameraRotationSpeed;

	ClampRotation(&Settings.xRotation);
	//ClampRotation(&Settings.yRotation);
	
	if(LocksEnabled){
		if(Settings.yRotation > Settings.MaxYRotLock)Settings.yRotation = Settings.MaxYRotLock;
		if(Settings.yRotation < Settings.MinYRotLock)Settings.yRotation = Settings.MinYRotLock;
		if(Settings.xRotation > Settings.MaxXRotLock)Settings.xRotation = Settings.MaxXRotLock;
		if(Settings.xRotation < Settings.MinXRotLock)Settings.xRotation = Settings.MinXRotLock;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////

void CCamera::IncTranslation(float xTrans, float yTrans, float zTrans)
{
	Settings.xTranslation += xTrans * Settings.CameraTranslationSpeed; 
	Settings.yTranslation += yTrans * Settings.CameraTranslationSpeed; 
	Settings.zTranslation += zTrans * Settings.CameraTranslationSpeed; 
};

////////////////////////////////////////////////////////////////////////////////////////////

void CCamera::SetLocks(float MinXLock, float MaxXLock, float MinYLock, float MaxYLock)
{
	Settings.MinXRotLock = MinXLock; 
	Settings.MaxXRotLock = MaxXLock; 
	Settings.MinYRotLock = MinYLock; 
	Settings.MaxYRotLock = MaxYLock;
}

void CCamera::EnableLocks()
{
	LocksEnabled = true;
}

void CCamera::DisableLocks()
{
	LocksEnabled = false;
}

bool CCamera::GetLocksStatus()
{
	return LocksEnabled;
}

//-----------------------------------------------------------------------------
// Get/Set viewport
//-----------------------------------------------------------------------------
void CCamera::GetViewport(int *pCoords)
{
	#ifdef COMPILE_FOR_DIRECTX9
	#else
		glGetIntegerv(GL_VIEWPORT, pCoords);
	#endif
}

//-----------------------------------------------------------------------------
// Set up OpenGL to draw in 2d
//-----------------------------------------------------------------------------
void CCamera::Set2DMode(int *pCoords, bool ReverseYAxis)
{
	static int l,t,w,h;

	if(!pCoords){
		l = 0;
		t = 0;
		w = pGameWindow->GetWidth();
		h = pGameWindow->GetHeight();
	} else {
		l = pCoords[0];
		t = pCoords[1];
		w = pCoords[2];
		h = pCoords[3];
	}

	#ifdef COMPILE_FOR_DIRECTX9
		D3DVIEWPORT9 vp = {l, t, w, h, 0, 1};
		dxdev->SetViewport(&vp);	

		D3DXMATRIX matView;
		D3DXMatrixIdentity(&matView);
		dxdev->SetTransform(D3DTS_VIEW, &matView);
		D3DXMATRIX matProjection;
		D3DXMatrixOrthoLH(&matProjection, (float)w, (float)h, 0, 1);
		dxdev->SetTransform(D3DTS_PROJECTION, &matProjection);
	#else
		glViewport(l, t, w, h);

		glMatrixMode(GL_PROJECTION); 
		glLoadIdentity();
		if(!ReverseYAxis){
			gluOrtho2D(l, w, t, h); 
		} else {
			gluOrtho2D(l, w, h, t); 
		}
		glMatrixMode(GL_MODELVIEW); 
	#endif
}

//-----------------------------------------------------------------------------
// Set up OpenGL to draw in 3d
//-----------------------------------------------------------------------------
void CCamera::Set3DMode(int *pCoords)
{
	static int l,t,w,h;
	static float r;

	if(!pCoords){
		l = 0;
		t = 0;
		w = pGameWindow->GetWidth();
		h = pGameWindow->GetHeight();
		r = pGameWindow->GetRatio();
	} else {
		l = pCoords[0];
		t = pCoords[1];
		w = pCoords[2];
		h = pCoords[3];
		r = (float)w / (float)h;
	}

	#ifdef COMPILE_FOR_DIRECTX9
		D3DXMATRIX matView;
		D3DXMatrixIdentity(&matView);
		dxdev->SetTransform(D3DTS_VIEW, &matView);

		D3DXMATRIX matProjection;
		D3DXMatrixPerspectiveFovLH(&matProjection, D3DXToRadian(45), r, Settings.NearPlane, Settings.FarPlane);
		dxdev->SetTransform(D3DTS_PROJECTION, &matProjection);
	#else
		glViewport(l, t, w, h);

		glMatrixMode(GL_PROJECTION); 
		glLoadIdentity(); 
		gluPerspective(45.0f, r, Settings.NearPlane, Settings.FarPlane); 
		glMatrixMode(GL_MODELVIEW); 
		glViewport(l, t, w, h);
	#endif
}

#ifndef _CCAMERA_H_
#define _CCAMERA_H_
#ifdef __cplusplus
//----------------------------------------------------------------------//
#define EXP_FUNC __declspec(dllexport)
//----------------------------------------------------------------------//
#ifdef COMPILE_FOR_DIRECTX9
#else 
#define COMPILE_FOR_OPENGL
#endif
//----------------------------------------------------------------------//
#include "Windows.h"
//----------------------------------------------------------------------//
#ifdef COMPILE_FOR_DIRECTX9
#include <d3d9.h>
#include <d3dx9.h>
#else
#include <gl\gl.h>
#include <gl\glu.h>
#endif
//----------------------------------------------------------------------//
#include "Vortez3DTypes.h"
#include "GameWindow.h"
#include "Extern.h"
//----------------------------------------------------------------------//

struct CCameraSettings {
	float NearPlane, FarPlane;
	float CameraDistance;
	float CameraRotationSpeed;
	float CameraTranslationSpeed;
	float CameraZoomSpeed;
	float xRotation, yRotation;
	float xTranslation, yTranslation, zTranslation;
	float MinXRotLock, MaxXRotLock;
	float MinYRotLock, MaxYRotLock;
};


class EXP_FUNC CCamera {
public:
	CCamera();
private:
	CCameraSettings Settings, SettingsBackup;
	bool LocksEnabled;
	void ClampRotation(float *r);
public:
	void Set2DMode(int *pCoords = NULL, bool ReverseYAxis = false);
	void Set3DMode(int *pCoords = NULL);
	
	void SaveSettings();
	void RestoreSettings();

	void ClearAll();
	void SetDefaultValue(CCameraSettings *pSettings);
	void SetDefaultValue(float Near, float Far, float RotX, float RotY, float TransX, float TransY, float TransZ, float CamDist, float CamRotSpd, float CamTransSpd, float CamZoomSpd, float MinXLock, float MaxXLock, float MinYLock, float MaxYLock);
	void SetCamera();

	void IncDistance(float CamDist);
	void IncRotation(float xRot, float yRot);
	void IncTranslation(float xTrans, float yTrans, float zTrans);

	void GetViewport(int *pCoords);

	float GetNearPlane(){return Settings.NearPlane;}
	float GetFarPlane(){return Settings.FarPlane;}
	float GetDistance(){return Settings.CameraDistance;}
	float GetRotationSpeed(){return Settings.CameraRotationSpeed;}
	float GetZoomSpeed(){return Settings.CameraZoomSpeed;}
	float GetXRotation(){return Settings.xRotation;}
	float GetYRotation(){return Settings.yRotation;}
	float GetXTranslation(){return Settings.xTranslation;}
	float GetYTranslation(){return Settings.yTranslation;}
	float GetZTranslation(){return Settings.zTranslation;}

	void SetNearPlane(float Near){Settings.NearPlane = Near;}
	void SetFarPlane(float Far){Settings.FarPlane = Far;}
	void SetDistance(float Dist){Settings.CameraDistance = Dist;}
	void SetRotationSpeed(float Speed){Settings.CameraRotationSpeed = Speed;}
	void SetZoomSpeed(float Speed){Settings.CameraZoomSpeed = Speed;}
	void SetRotation(float xRot, float yRot){Settings.xRotation = xRot; Settings.yRotation = yRot;}
	void SetTranslation(float xTrans, float yTrans, float zTrans){Settings.xTranslation = xTrans; Settings.yTranslation = yTrans; Settings.zTranslation = zTrans;}
	
	void SetLocks(float MinXLock, float MaxXLock, float MinYLock, float MaxYLock);
	void EnableLocks();
	void DisableLocks();
	bool GetLocksStatus();
};


#endif
#endif //--_CCAMERA_H_

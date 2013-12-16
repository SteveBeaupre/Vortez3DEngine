#include "Model.h"

CModel::CModel()
{
	#ifdef COMPILE_FOR_DIRECTX9
		pBuffer      = NULL;
		pIndexBuffer = NULL;
		ZeroMemory(&pTexture[0], sizeof(LPDIRECT3DTEXTURE9) * MAX_TEX_CHANNEL);

		VertexFormat     = 0;
		VertexFormatSize = 0;
	#else
		pVBO = (CVBO*)pvVBO;
		ZeroMemory(&TextureID[0], sizeof(UINT) * MAX_TEX_CHANNEL);
	#endif

	ZeroMemory(Name, 32);
	ZeroMemory(&Color, sizeof(float3));
	ZeroMemory(AvailableArrays, 8);

	Indexed           = false;
	IndicesCount      = 0;
	VerticesCount     = 0;
	FacesCount        = 0;
	
	pIndexArray       = NULL;
	pVertsArray       = NULL;
	pColorArray       = NULL;
	pTexVertsArray    = NULL;
	pAuxTexVertsArray = NULL;
	pNormalsArray     = NULL;
	pFogArray         = NULL;
}

#ifdef COMPILE_FOR_OPENGL
void EnableVAs(CModel *pModel, DWORD  dw)
{
	UINT CurrentTU = GetCurrentTextureUnit();

	if((dw & VA_VERTEX) > 0 && pModel->pVertsArray != NULL)
		glEnableClientState(GL_VERTEX_ARRAY);
	if((dw & VA_COLOR_ARRAY) > 0 && pModel->pColorArray != NULL)
		glEnableClientState(GL_COLOR_ARRAY);
	if((dw & VA_TEXTURE) > 0 && pModel->pTexVertsArray != NULL){
		SetCurrentTextureUnit(0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	if((dw & VA_AUXTEXTURE) > 0 && pModel->pAuxTexVertsArray != NULL){
		SetCurrentTextureUnit(1);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	if((dw & VA_NORMALS) > 0 && pModel->pNormalsArray != NULL)
		glEnableClientState(GL_NORMAL_ARRAY);
	if((dw & VA_FOG) > 0 && pModel->pFogArray != NULL)
		glEnableClientState(FOG_COORDINATE_ARRAY_EXT);
	
	SetCurrentTextureUnit(CurrentTU);
}

void DisableVAs(CModel *pModel, DWORD dw)
{
	UINT CurrentTU = GetCurrentTextureUnit();

	if((dw & VA_VERTEX) > 0 && pModel->pVertsArray != NULL)
		glDisableClientState(GL_VERTEX_ARRAY);
	if((dw & VA_COLOR_ARRAY) > 0 && pModel->pColorArray != NULL)
		glDisableClientState(GL_COLOR_ARRAY);
	if((dw & VA_TEXTURE) > 0 && pModel->pTexVertsArray != NULL){
		SetCurrentTextureUnit(0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	if((dw & VA_AUXTEXTURE) > 0 && pModel->pAuxTexVertsArray != NULL){
		SetCurrentTextureUnit(1);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	if((dw & VA_NORMALS) > 0 && pModel->pNormalsArray != NULL)
		glDisableClientState(GL_NORMAL_ARRAY);
	if((dw & VA_FOG) > 0 && pModel->pFogArray != NULL)
		glDisableClientState(FOG_COORDINATE_ARRAY_EXT);
	
	SetCurrentTextureUnit(CurrentTU);
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CModel::EnableVAs(DWORD dw)
{
	::EnableVAs(this, dw);
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CModel::DisableVAs(DWORD dw)
{
	::DisableVAs(this, dw);
}

//-----------------------------------------------------------------------------
// Return the current TU
//-----------------------------------------------------------------------------
UINT GetCurrentTextureUnit()
{
	int CurrentActiveTextureUnit = 0;
	glGetIntegerv(GL_ACTIVE_TEXTURE_ARB, &CurrentActiveTextureUnit);
	return CurrentActiveTextureUnit - GL_TEXTURE0_ARB;
}

//-----------------------------------------------------------------------------
// Set the current TU
//-----------------------------------------------------------------------------
void SetCurrentTextureUnit(UINT Val)
{
	if(ActiveTextureARB && ClientActiveTextureARB){
		ActiveTextureARB(GL_TEXTURE0_ARB + Val);
		ClientActiveTextureARB(GL_TEXTURE0_ARB + Val);
	}
}
#endif

#ifdef COMPILE_FOR_DIRECTX9
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CModel::MakeFVF(DWORD *pFormat, DWORD *pFormatSize)
{
	// Vertices 
	*pFormat = D3DFVF_XYZ;
	*pFormatSize += sizeof(float3);

	// Normals 
	if(AvailableArrays[VA_NORMAL_ARRAY]){
		*pFormat |= D3DFVF_NORMAL;
		*pFormatSize += sizeof(float3);
	}

	// Colors 
	*pFormat |= D3DFVF_DIFFUSE;
	*pFormatSize += sizeof(DWORD);

	// Tex Coords 1 
	if(AvailableArrays[VA_TEX1_ARRAY]){
		*pFormat |= D3DFVF_TEX1;
		*pFormatSize += sizeof(float2);
	}

	// Tex Coords 2 
	if(AvailableArrays[VA_TEX2_ARRAY]){
		*pFormat |= D3DFVF_TEX2;
		*pFormatSize += sizeof(float2);
	}
}
#endif

void CModel::LoadTexture(UINT Channel, char *fname, bool UseMipmap, int FilterType, int WrapMode, int AlphaChannelType)
{
	#ifdef COMPILE_FOR_DIRECTX9
		if(!pTexture[Channel]){
			D3DXCreateTextureFromFile(dxdev, fname, &pTexture[Channel]);
			dxdev->SetTexture(0, pTexture[Channel]);
			dxdev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			dxdev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);			
			dxdev->SetSamplerState(0, D3DSAMP_MAGFILTER, (DWORD)FilterType);
			dxdev->SetSamplerState(0, D3DSAMP_MINFILTER, (DWORD)FilterType);
		}
	#else
		TextureLoader.UseMipmaps = UseMipmap;
		TextureLoader.FilterType = FilterType;
		TextureLoader.WrapMode   = WrapMode;
		TextureID[Channel] = TextureLoader.LoadFromFile(fname);
	#endif
}

#ifdef COMPILE_FOR_DIRECTX9
void CModel::SetTexturePointer(UINT Channel, LPDIRECT3DTEXTURE9 pTex)
{
	pTexture[Channel] = pTex;
}

void CModel::SetCurrentTexture(UINT Channel)
{
	if(pTexture[Channel])
		dxdev->SetTexture(Channel, pTexture[Channel]);
}

#else

UINT CModel::GetTextureID(UINT Channel)
{
	return TextureID[Channel];
}

void CModel::SetTextureID(UINT Channel, UINT TexID)
{
	TextureID[Channel] = TexID;
}

void CModel::BindTexture(UINT Channel)
{
	glBindTexture(GL_TEXTURE_2D, TextureID[Channel]);
}
#endif

#ifdef COMPILE_FOR_DIRECTX9
//-----------------------------------------------------------------------------
// Create the directx vertex buffer
//-----------------------------------------------------------------------------
void CModel::CreateDXVertexBuffer()
{
	if(!pBuffer && pVertsArray != NULL){

		MakeFVF(&VertexFormat, &VertexFormatSize);

		DWORD NumElements = Indexed ? IndicesCount : VerticesCount;
		DWORD BufSize     = NumElements * VertexFormatSize;

		if(Indexed){
			dxdev->CreateIndexBuffer(sizeof(UINT) * VerticesCount, 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &pIndexBuffer, NULL);

			VOID* pVoid = NULL;
			pIndexBuffer->Lock(0, 0, (void**)&pVoid, 0);

			UINT *pBuf = (UINT*)pVoid;
			for(UINT Cpt = 0; Cpt < VerticesCount; Cpt++)
				memcpy(&pBuf[Cpt], &pIndexArray[Cpt], sizeof(UINT));

			pIndexBuffer->Unlock(); 
		}

		dxdev->CreateVertexBuffer(BufSize, 0, VertexFormat, D3DPOOL_MANAGED, &pBuffer, NULL);
		VOID* pVoid = NULL;
		pBuffer->Lock(0, 0, (void**)&pVoid, 0);

		BYTE *pBuf = (BYTE*)pVoid;
		for(UINT Cpt = 0; Cpt < NumElements; Cpt++){

			memcpy(pBuf, &pVertsArray[Cpt], sizeof(float3));
			pBuf += sizeof(float3);

			if(AvailableArrays[VA_NORMAL_ARRAY]){
				memcpy(pBuf, &pNormalsArray[Cpt], sizeof(float3));
				pBuf += sizeof(float3);
			}
			if(AvailableArrays[VA_COLOR_ARRAY]){
				float3 fCol = pColorArray[Cpt];
				DWORD dwCol = D3DCOLOR_XRGB((BYTE)(fCol.x * 255.0f), (BYTE)(fCol.y * 255.0f), (BYTE)(fCol.z * 255.0f));
				memcpy(pBuf, &dwCol, sizeof(DWORD));
				pBuf += sizeof(DWORD);
			} else {
				DWORD dwCol = D3DCOLOR_XRGB((BYTE)(Color.x * 255.0f), (BYTE)(Color.y * 255.0f), (BYTE)(Color.z * 255.0f));
				memcpy(pBuf, &dwCol, sizeof(DWORD)); 
				pBuf += sizeof(DWORD);
			}
			if(AvailableArrays[VA_TEX1_ARRAY]){
				memcpy(pBuf, &pTexVertsArray[Cpt], sizeof(float2));
				pBuf += sizeof(float2);
			}
			if(AvailableArrays[VA_TEX2_ARRAY]){
				memcpy(pBuf, &pAuxTexVertsArray[Cpt], sizeof(float2));
				pBuf += sizeof(float2);
			}
		}
		pBuffer->Unlock(); 
	}
}

//-----------------------------------------------------------------------------
// Release the directx vertex buffer
//-----------------------------------------------------------------------------
void CModel::ReleaseDXVertexBuffer()
{
	SAFE_RELEASE(pIndexBuffer);
	SAFE_RELEASE(pBuffer);
	VertexFormat = 0;
	VertexFormatSize = 0;
}
#endif

//-----------------------------------------------------------------------------
// Load the 3d model 
//-----------------------------------------------------------------------------
bool CModel::Load(char *pFName, bool bIndexed)
{
	CModelLoader ModelLoader;
	return ModelLoader.LoadModel(pFName, this, bIndexed);
}

//-----------------------------------------------------------------------------
// Free the 3d model 
//-----------------------------------------------------------------------------
void CModel::Free()
{
	#ifdef COMPILE_FOR_DIRECTX9
		ReleaseDXVertexBuffer();
	#else
		#ifndef NO_VBO
			if(pVBO->IsVBOSupported)
				pVBO->RemoveModelFromVBO(this);
		#endif
	#endif

	//Free model data
	FreeDynamicsArrays();
}

//-----------------------------------------------------------------------------
// Free the model's data memory
//-----------------------------------------------------------------------------
void CModel::FreeDynamicsArrays()
{
	SAFE_DELETE_ARRAY(pFogArray);
	SAFE_DELETE_ARRAY(pNormalsArray);
	SAFE_DELETE_ARRAY(pAuxTexVertsArray);
	SAFE_DELETE_ARRAY(pTexVertsArray);
	SAFE_DELETE_ARRAY(pColorArray);
	SAFE_DELETE_ARRAY(pVertsArray);
	SAFE_DELETE_ARRAY(pIndexArray);
}

#ifdef COMPILE_FOR_OPENGL
//-----------------------------------------------------------------------------
void CModel::BindVAs(DWORD dw)
{
	UINT CurrentTU = GetCurrentTextureUnit();

	#ifndef NO_VBO
		if(pVBO->IsVBOSupported){
			if((dw & VA_VERTEX) > 0 && pVertsArray != NULL){
				pVBO->glBindBufferARB(GL_ARRAY_BUFFER_ARB, VertsVBO_ID);
				glVertexPointer(3, GL_FLOAT, 0, (char *)NULL);
			}
			if((dw & VA_COLOR) > 0 && pColorArray != NULL){
				pVBO->glBindBufferARB(GL_ARRAY_BUFFER_ARB, ColorVBO_ID);
				glColorPointer(3, GL_FLOAT, 0, (char *)NULL);
			}
			if((dw & VA_TEXTURE) > 0 && pTexVertsArray != NULL){
				SetCurrentTextureUnit(0);
				pVBO->glBindBufferARB(GL_ARRAY_BUFFER_ARB, TexVertsVBO_ID);
				glTexCoordPointer(2, GL_FLOAT, 0, (char *)NULL);
			}
			if((dw & VA_AUXTEXTURE) > 0 && pAuxTexVertsArray != NULL){ // MUST be the first checked and 2ndairy texture should be enabled...
				SetCurrentTextureUnit(1);
				pVBO->glBindBufferARB(GL_ARRAY_BUFFER_ARB, AuxTexVertsVBO_ID);
				glTexCoordPointer(2, GL_FLOAT, 0, (char *)NULL);
			}
			if((dw & VA_NORMALS) > 0 && pNormalsArray != NULL){
				pVBO->glBindBufferARB(GL_ARRAY_BUFFER_ARB, NormalVBO_ID);
				glNormalPointer(GL_FLOAT, 0, (char *)NULL);
			}
			if((dw & VA_FOG) > 0 && pFogArray != NULL){
				pVBO->glBindBufferARB(GL_ARRAY_BUFFER_ARB, FogVBO_ID);
				FogCoordPointer(GL_FLOAT, 0, (char *)NULL);
			}
		} else {
	#endif
			if((dw & VA_VERTEX) > 0 && pVertsArray != NULL)
				glVertexPointer(3, GL_FLOAT, 0, pVertsArray);
			if((dw & VA_COLOR) > 0 && pColorArray != NULL)
				glColorPointer(3, GL_FLOAT, 0, pColorArray);
			if((dw & VA_TEXTURE) > 0 && pTexVertsArray != NULL){
				SetCurrentTextureUnit(0);
				glTexCoordPointer(2, GL_FLOAT, 0, pTexVertsArray);
			}
			if((dw & VA_AUXTEXTURE) > 0 && pAuxTexVertsArray != NULL){
				SetCurrentTextureUnit(1);
				glTexCoordPointer(2, GL_FLOAT, 0, pAuxTexVertsArray);
			}
			if((dw & VA_NORMALS) > 0 && pNormalsArray != NULL)
				glNormalPointer(GL_FLOAT, 0, pNormalsArray);
			if((dw & VA_FOG) > 0 && pFogArray != NULL)
				FogCoordPointer(GL_FLOAT, 0, pFogArray);
	#ifndef NO_VBO
		}
	#endif

	SetCurrentTextureUnit(CurrentTU);
}
#endif

//-----------------------------------------------------------------------------
// Draw the model...
//-----------------------------------------------------------------------------
void CModel::Draw()
{
	#ifdef COMPILE_FOR_DIRECTX9
			dxdev->SetFVF(VertexFormat);
			dxdev->SetStreamSource(0, pBuffer, 0, VertexFormatSize);
			switch(IsIndexed())
			{
			case false: 
				dxdev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, VerticesCount / 3); 
				break;
			default:
				dxdev->SetIndices(pIndexBuffer);
				dxdev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,IndicesCount,0,VerticesCount / 3);
				break;
			}
			dxdev->SetTexture(0, NULL);
	#else
		switch(IsIndexed())
		{
		case false: glDrawArrays(GL_TRIANGLES, 0, VerticesCount); break;
		default:    glDrawElements(GL_TRIANGLES, IndicesCount, GL_UNSIGNED_INT, pIndexArray); break;
		}
	#endif

	*pPolygonsDrawed += FacesCount;
}

//-----------------------------------------------------------------------------
// Draw the model...
//-----------------------------------------------------------------------------
void CModel::Draw(DWORD flags)
{
	#ifdef COMPILE_FOR_DIRECTX9

	#else
		DisableVAs(VA_ALL);
		EnableVAs(flags);
		BindVAs(flags);
		switch(IsIndexed())
		{
		case false: glDrawArrays(GL_TRIANGLES, 0, VerticesCount); break;
		default:    glDrawElements(GL_TRIANGLES, VerticesCount, GL_UNSIGNED_INT, pIndexArray); break;
		}
		DisableVAs(flags);
	#endif

	*pPolygonsDrawed += FacesCount;
}

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//

CModelLoader::CModelLoader()
{
	#ifdef COMPILE_FOR_OPENGL
		pVBO = (CVBO*)pvVBO;
	#endif
}


bool CModelLoader::LoadModel(char *pFName, CModel *pModel, bool bIndexed)
{
	return LoadModelsSet(pFName, pModel, 1, bIndexed);
}

void CModelLoader::FreeModel(CModel *pModel)
{
	FreeModelsSet(pModel, 1);
}

bool CModelLoader::LoadModelsSet(char *fname, CModel *pModelsList, UINT MaxModels, bool bIndexed)
{
	static const int SigSize = 16;
	static const int VerNumSize = 16;
	static const int MaxNameLen = 64;
	static const int MaxColors = 4;
	static const int AvailArraySize = 8;

	//CModel *pModel = pModelsList++;
	void *pList = pModelsList;

	if(MaxModels == 0)
		MaxModels = 0xFFFFFFFF;
	//////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////Open the file/////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////

	//Open the file
	CM3DFileIO f;
	if(!f.OpenForReading(fname)){
		char c[256];
		sprintf(c, "Unable to open the file \"%s\"", fname);
		MessageBox(0, c, "Error!", 0);
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////Read the main header/////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////

	// Verify the signature
	BYTE Signature[SigSize];
	ZeroMemory(Signature, SigSize);
	f.Read(&Signature[0], SigSize);

	// Verify the signature
	if(memcmp(Signature, SIGNATURE, SigSize) != 0){
		MessageBox(NULL, "File signature not found, invalid file...", "Error!", 0);
		f.Close();
		return false;
	}

	// Verify the version number
	BYTE VerNum[VerNumSize];
	ZeroMemory(VerNum, VerNumSize);
	f.Read(&VerNum[0], VerNumSize);

	// Verify the signature
	if(memcmp(VerNum, VERSION_NUMBER, VerNumSize) != 0){
		MessageBox(NULL, "Invalid version number, please re-save the file...", "Error!", 0);
		f.Close();
		return false;
	}

	// Read the Obj. Count
	DWORD ObjCount = 0;
	f.Read(&ObjCount, sizeof(DWORD));

	// Check if the file is compressed or not
	DWORD IsCompressed = 0;
	f.Read(&IsCompressed, sizeof(DWORD));

	// Save the number of models loaded so far...
	UINT NumModelsLoaded = 0;

	// Start loading models one by one...
	do 
	{
		CModel *pModel = NULL;
		_asm {
			push eax                 
			push ebx                 // save registers
			mov ebx, pList           // put the list pointer in ebx
			mov eax, dword ptr [ebx] // put the CModel pointer in eax
			mov pModel, eax          // pModel point to the CModel pointer
			add ebx, 4               // inc ebx by 4
			mov pList, ebx           // pList point to the next CModel pointer of the list
			pop ebx                  // restore registers
			pop eax
		}
		//////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////Read the obj. header/////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////

		pModel->Indexed = bIndexed;

		// Read the object name
		ZeroMemory(&pModel->Name[0],  MaxNameLen);
		f.Read(&pModel->Name[0], MaxNameLen);
	
		// Read the object color
		BYTE ObjCol[MaxColors];
		f.Read(&ObjCol[0], MaxColors);
		//Convert and save it into float
		pModel->Color.x = (float)ObjCol[3] / 255.0f;
		pModel->Color.y = (float)ObjCol[2] / 255.0f;
		pModel->Color.z = (float)ObjCol[1] / 255.0f;

		// Read the available arrays we can use
		f.Read(&pModel->AvailableArrays[0], AvailArraySize);

		//Read the number of indices, vertices and faces we have
		f.Read(&pModel->IndicesCount,  sizeof(UINT));
		f.Read(&pModel->VerticesCount, sizeof(UINT));
		f.Read(&pModel->FacesCount,    sizeof(UINT));

		// Pass some useless data
		UINT TexMapVertsCount[5];
		f.ReadDataBlock((BYTE*)&TexMapVertsCount[0], sizeof(UINT) * 5, FALSE);

		//////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////Allocate memory for VAs////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////

		//Allocate some memory block to hold the model's data
		if(pModel->Indexed){
			CREATE_ARRAY_MACRO(pModel->pIndexArray, UINT,   pModel->VerticesCount)
			CREATE_ARRAY_MACRO(pModel->pVertsArray, float3, pModel->IndicesCount)
			CREATE_OPTIONAL_ARRAY_MACRO(VA_COLOR_ARRAY,  pModel->pColorArray,       float3, pModel->IndicesCount)
			CREATE_OPTIONAL_ARRAY_MACRO(VA_TEX1_ARRAY,   pModel->pTexVertsArray,    float2, pModel->IndicesCount)
			CREATE_OPTIONAL_ARRAY_MACRO(VA_TEX2_ARRAY,   pModel->pAuxTexVertsArray, float2, pModel->IndicesCount)
			CREATE_OPTIONAL_ARRAY_MACRO(VA_NORMAL_ARRAY, pModel->pNormalsArray,     float3, pModel->IndicesCount)
			CREATE_OPTIONAL_ARRAY_MACRO(VA_FOG_ARRAY,    pModel->pFogArray,         float,  pModel->IndicesCount)
		} else {
			CREATE_ARRAY_MACRO(pModel->pVertsArray, float3, pModel->VerticesCount)
			CREATE_OPTIONAL_ARRAY_MACRO(VA_COLOR_ARRAY,  pModel->pColorArray,       float3, pModel->VerticesCount)
			CREATE_OPTIONAL_ARRAY_MACRO(VA_TEX1_ARRAY,   pModel->pTexVertsArray,    float2, pModel->VerticesCount)
			CREATE_OPTIONAL_ARRAY_MACRO(VA_TEX2_ARRAY,   pModel->pAuxTexVertsArray, float2, pModel->VerticesCount)
			CREATE_OPTIONAL_ARRAY_MACRO(VA_NORMAL_ARRAY, pModel->pNormalsArray,     float3, pModel->VerticesCount)
			CREATE_OPTIONAL_ARRAY_MACRO(VA_FOG_ARRAY,    pModel->pFogArray,         float,  pModel->VerticesCount)
		}

		//////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////Read the obj. data/////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////

		if(pModel->Indexed){
			// Read the indexed data...
			f.ReadDataBlock((BYTE*)&pModel->pIndexArray[0], pModel->VerticesCount * sizeof(UINT), IsCompressed);
			f.ReadDataBlock((BYTE*)&pModel->pVertsArray[0], pModel->IndicesCount * sizeof(float3), IsCompressed);
			if(pModel->AvailableArrays[VA_COLOR_ARRAY] != 0)
				f.ReadDataBlock((BYTE*)&pModel->pColorArray[0], pModel->IndicesCount * sizeof(float3), IsCompressed);
			if(pModel->AvailableArrays[VA_TEX1_ARRAY] != 0)
				f.ReadDataBlock((BYTE*)&pModel->pTexVertsArray[0], pModel->IndicesCount * sizeof(float2), IsCompressed);
			if(pModel->AvailableArrays[VA_TEX2_ARRAY] != 0)
				f.ReadDataBlock((BYTE*)&pModel->pAuxTexVertsArray[0], pModel->IndicesCount * sizeof(float2), IsCompressed);
			if(pModel->AvailableArrays[VA_TEX3_ARRAY] != 0)
				f.PassDataBlock(pModel->IndicesCount * sizeof(float2), IsCompressed);
			if(pModel->AvailableArrays[VA_TEX4_ARRAY] != 0)
				f.PassDataBlock(pModel->IndicesCount * sizeof(float2), IsCompressed);
			if(pModel->AvailableArrays[VA_NORMAL_ARRAY] != 0)
				f.ReadDataBlock((BYTE*)&pModel->pNormalsArray[0], pModel->IndicesCount * sizeof(float3), IsCompressed);
			if(pModel->AvailableArrays[VA_FOG_ARRAY] != 0)
				f.ReadDataBlock((BYTE*)&pModel->pFogArray[0], pModel->IndicesCount * sizeof(float), IsCompressed);

			// Pass the non-indexed data...
			f.PassDataBlock(pModel->VerticesCount * sizeof(float3), IsCompressed);
			if(pModel->AvailableArrays[VA_COLOR_ARRAY] != 0)
				f.PassDataBlock(pModel->VerticesCount * sizeof(float3), IsCompressed);
			if(pModel->AvailableArrays[VA_TEX1_ARRAY] != 0)
				f.PassDataBlock(pModel->VerticesCount * sizeof(float2), IsCompressed);
			if(pModel->AvailableArrays[VA_TEX2_ARRAY] != 0)
				f.PassDataBlock(pModel->VerticesCount * sizeof(float2), IsCompressed);
			if(pModel->AvailableArrays[VA_TEX3_ARRAY] != 0)
				f.PassDataBlock(pModel->VerticesCount * sizeof(float2), IsCompressed);
			if(pModel->AvailableArrays[VA_TEX4_ARRAY] != 0)
				f.PassDataBlock(pModel->VerticesCount * sizeof(float2), IsCompressed);
			if(pModel->AvailableArrays[VA_NORMAL_ARRAY] != 0)
				f.PassDataBlock(pModel->VerticesCount * sizeof(float3), IsCompressed);
			if(pModel->AvailableArrays[VA_FOG_ARRAY] != 0)
				f.PassDataBlock(pModel->VerticesCount * sizeof(float), IsCompressed);
		} else { 
			// Pass the indexed data...
			f.PassDataBlock(pModel->VerticesCount * sizeof(UINT), IsCompressed);
			f.PassDataBlock(pModel->IndicesCount * sizeof(float3), IsCompressed);
			if(pModel->AvailableArrays[VA_COLOR_ARRAY] != 0)
				f.PassDataBlock(pModel->IndicesCount * sizeof(float3), IsCompressed);
			if(pModel->AvailableArrays[VA_TEX1_ARRAY] != 0)
				f.PassDataBlock(pModel->IndicesCount * sizeof(float2), IsCompressed);
			if(pModel->AvailableArrays[VA_TEX2_ARRAY] != 0)
				f.PassDataBlock(pModel->IndicesCount * sizeof(float2), IsCompressed);
			if(pModel->AvailableArrays[VA_TEX3_ARRAY] != 0)
				f.PassDataBlock(pModel->IndicesCount * sizeof(float2), IsCompressed);
			if(pModel->AvailableArrays[VA_TEX4_ARRAY] != 0)
				f.PassDataBlock(pModel->IndicesCount * sizeof(float2), IsCompressed);
			if(pModel->AvailableArrays[VA_NORMAL_ARRAY] != 0)
				f.PassDataBlock(pModel->IndicesCount * sizeof(float3), IsCompressed);
			if(pModel->AvailableArrays[VA_FOG_ARRAY] != 0)
				f.PassDataBlock(pModel->IndicesCount * sizeof(float), IsCompressed);

			//////////////////////////////////////////////

			// Read the non-indexed data...
			f.ReadDataBlock((BYTE*)&pModel->pVertsArray[0], pModel->VerticesCount * sizeof(float3), IsCompressed);
			if(pModel->AvailableArrays[VA_COLOR_ARRAY] != 0)
				f.ReadDataBlock((BYTE*)&pModel->pColorArray[0], pModel->VerticesCount * sizeof(float3), IsCompressed);
			if(pModel->AvailableArrays[VA_TEX1_ARRAY] != 0)
				f.ReadDataBlock((BYTE*)&pModel->pTexVertsArray[0], pModel->VerticesCount * sizeof(float2), IsCompressed);
			if(pModel->AvailableArrays[VA_TEX2_ARRAY] != 0)
				f.ReadDataBlock((BYTE*)&pModel->pAuxTexVertsArray[0], pModel->VerticesCount * sizeof(float2), IsCompressed);
			if(pModel->AvailableArrays[VA_TEX3_ARRAY] != 0)
				f.PassDataBlock(pModel->VerticesCount * sizeof(float2), IsCompressed);
			if(pModel->AvailableArrays[VA_TEX4_ARRAY] != 0)
				f.PassDataBlock(pModel->VerticesCount * sizeof(float2), IsCompressed);
			if(pModel->AvailableArrays[VA_NORMAL_ARRAY] != 0)
				f.ReadDataBlock((BYTE*)&pModel->pNormalsArray[0], pModel->VerticesCount * sizeof(float3), IsCompressed);
			if(pModel->AvailableArrays[VA_FOG_ARRAY] != 0)
				f.ReadDataBlock((BYTE*)&pModel->pFogArray[0], pModel->VerticesCount * sizeof(float), IsCompressed);
		}
		
		// Skip the 3ds max importer data...
		f.PassDataBlock(pModel->VerticesCount, IsCompressed);
		for(int TexIndx = 0; TexIndx < 5; TexIndx++){
			if(pModel->AvailableArrays[VA_COLOR_ARRAY + TexIndx]){
				f.PassDataBlock(pModel->VerticesCount * sizeof(UINT), IsCompressed);
				f.PassDataBlock(TexMapVertsCount[TexIndx] * sizeof(float3), IsCompressed);			
			}
		}

		#ifdef COMPILE_FOR_DIRECTX9
			pModel->CreateDXVertexBuffer();
		#else
			//Add the model to the VBO if avalaible
			#ifndef NO_VBO
			if(pVBO->IsVBOSupported)
				pVBO->GenAndBindModelToVBO(pModel);
			#endif
		#endif

		// Inc. the number of model loaded counter
		NumModelsLoaded++;

		// Make sure we arent loading more models than requested...
		if(NumModelsLoaded >= MaxModels)
			break;
	} 
	while(NumModelsLoaded <= ObjCount);

	// Close the file
	f.Close();

	return true;
}

void CModelLoader::FreeModelsSet(CModel *pModelsList, UINT NumModels)
{
	CModel *pModel = NULL;
	pModel = pModelsList;
	for(UINT Cpt = 0; Cpt < NumModels; Cpt++){
		pModel->Free();
		pModel++;
	}
}

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//

void CM3DFileIO::PassCompressedDataBlock()
{
	DWORD UncompressedSize = 0;
	DWORD CompressedSize   = 0;
	DWORD NumEncodedChar   = 0;

	this->Read(&UncompressedSize, sizeof(DWORD));
	this->Read(&CompressedSize,   sizeof(DWORD));
	this->Read(&NumEncodedChar,   sizeof(DWORD));

	///////////////////////////////////////////////////////////

	DWORD BinTableLen = NumEncodedChar * 8;
	BYTE *pBinTable = new BYTE[BinTableLen];
	this->Read(&pBinTable[0], BinTableLen);
	SAFE_DELETE_ARRAY(pBinTable);

	///////////////////////////////////////////////////////////

	PassDataBlock(CompressedSize, FALSE);
}

void CM3DFileIO::ReadDataBlock(BYTE* pData, DWORD Size, DWORD IsCompressed)
{
	if(!IsCompressed){
		this->Read(pData, Size);
	} else {
		DWORD UncompressedSize = 0;
		DWORD CompressedSize   = 0;
		DWORD NumEncodedChar   = 0;

		this->Read(&UncompressedSize, sizeof(DWORD));
		this->Read(&CompressedSize,   sizeof(DWORD));
		this->Read(&NumEncodedChar,   sizeof(DWORD));

		///////////////////////////////////////////////////////////

		DWORD BinTableLen = NumEncodedChar * 8;
		BYTE *pBinTable = new BYTE[BinTableLen];
		this->Read(&pBinTable[0], BinTableLen);

		///////////////////////////////////////////////////////////
		
		BYTE *pCompressedData = new BYTE[CompressedSize];
		this->Read(&pCompressedData[0], CompressedSize);
		
		///////////////////////////////////////////////////////////

		CCompressor Compressor;
		Compressor.DecodeFromMemory(&pCompressedData[0], &pData[0], UncompressedSize, CompressedSize, NumEncodedChar, &pBinTable[0]);
		
		///////////////////////////////////////////////////////////

		SAFE_DELETE_ARRAY(pCompressedData);
		SAFE_DELETE_ARRAY(pBinTable);
	}	
}

void CM3DFileIO::PassDataBlock(DWORD Size, DWORD IsCompressed)
{
	if(!IsCompressed){
		BYTE* pTmpBuf = new BYTE[Size];
		this->Read(&pTmpBuf[0], Size);
		SAFE_DELETE_ARRAY(pTmpBuf);
	} else {
		PassCompressedDataBlock();
	}
}
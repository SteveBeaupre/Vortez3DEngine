/**********************************************************************
 *<
	FILE: Model3DImport.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "Model3DImport.h"

#ifdef _VS2010_
#define MODEL3DIMPORT_CLASS_ID	Class_ID(0x814d92b4, 0x2ca32b77)
#else
#define MODEL3DIMPORT_CLASS_ID	Class_ID(0xdf1fdbd, 0x114ded1b)
#endif


class Model3DImport : public SceneImport {
	public:


		static HWND hParams;

		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box
		int				DoImport(const TCHAR *name,ImpInterface *i,Interface *gi, BOOL suppressPrompts=FALSE);	// Import file
		
		//Constructor/Destructor

		Model3DImport();
		~Model3DImport();		

		FILE *InF;
		bool ReadSignature();
		WORD ReadFileFormatVersion();

		DWORD ReadNumObject();
		DWORD ReadCompressedFlag();

		void ConvertObjColorFlag(DWORD ObjColor, DWORD* pBuf);
		void FillVerticesData(Mesh *pMesh, CModelDataStruct *pModelData);
		void FillTexData(Mesh *pMesh, CModelDataStruct *pModelData);

		BOOL ReadOldFile(Mesh *pMesh, char *pNodeName);
		BOOL ReadObject(Mesh *pMesh, char *pNodeName, DWORD *pNodeColor, DWORD IsCompressed, WORD Version);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Model3DImportClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new Model3DImport(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_IMPORT_CLASS_ID; }
	Class_ID		ClassID() { return MODEL3DIMPORT_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("Model3DImport"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};



static Model3DImportClassDesc Model3DImportDesc;
ClassDesc2* GetModel3DImportDesc() { return &Model3DImportDesc; }


BOOL CALLBACK Model3DImportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static Model3DImport *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (Model3DImport *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;
	}
	return FALSE;
}


//--- Model3DImport -------------------------------------------------------
Model3DImport::Model3DImport()
{

}

Model3DImport::~Model3DImport() 
{

}

int Model3DImport::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *Model3DImport::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("M3D");
}

const TCHAR *Model3DImport::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("Model3D File Importer 2.0");
}
	
const TCHAR *Model3DImport::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("M3D File Importer 2.0");
}

const TCHAR *Model3DImport::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("Vortez");
}

const TCHAR *Model3DImport::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("Enjoy!");
}

const TCHAR *Model3DImport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *Model3DImport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int Model3DImport::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 200;
}

void Model3DImport::ShowAbout(HWND hWnd)
{			
	// Optional
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WORD ExtractFileVersion(char *ver)
{
	ver[1] = NULL;
	BYTE Hi = atoi(ver);
	ver += 2;
	BYTE Lo = atoi(ver);

	return (Hi<<8) | Lo;
}

bool Model3DImport::ReadSignature()
{
	const int BufSize = 16;
	BYTE Buf[BufSize];
	ZeroMemory(Buf, BufSize);

	//Read the signature
	fread(&Buf[0], 1, BufSize, InF);

	// Verify if the 2 signatures match
	if(memcmp(&Buf[0], SIGNATURE, BufSize) != 0){
		MessageBox(NULL, "Invaled file, signature not found.", "Error!", 0);
		return false;
	}

	return true;
}

WORD Model3DImport::ReadFileFormatVersion()
{
	const int BufSize = 16;
	BYTE Buf[BufSize];
	ZeroMemory(&Buf[0], BufSize);

	//Read the signature
	fread(&Buf[0], 1, BufSize, InF);

	// Verify if the 2 signatures match
	if(memcmp(&Buf[0], FILE_FORMAT_VERSION, BufSize) != 0){
		// Assuming version 1.0...
		return VER_1_0;
	}

	return ExtractFileVersion((char*)&Buf[12]);
}

DWORD Model3DImport::ReadNumObject()
{
	DWORD NumObj = 0;
	fread(&NumObj, 1, sizeof(DWORD), InF);
	return NumObj;
}

DWORD Model3DImport::ReadCompressedFlag()
{
	DWORD IsCompressed = 0;
	fread(&IsCompressed, 1, sizeof(DWORD), InF);
	return IsCompressed;
}

void Model3DImport::ConvertObjColorFlag(DWORD ObjColor, DWORD* pBuf)
{
	UINT MeshCol = ObjColor;

	// Convert from XGBR to RGBX
	BYTE *pObjColor = (BYTE*)&ObjColor;
	BYTE *pMeshCol = (BYTE*)&MeshCol;
	pMeshCol[0] = pObjColor[3];
	pMeshCol[1] = pObjColor[2];
	pMeshCol[2] = pObjColor[1];
	pMeshCol[3] = pObjColor[0];

	// Write mesh color
	memcpy(pBuf, &MeshCol, sizeof(UINT));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Model3DImport::FillVerticesData(Mesh *pMesh, CModelDataStruct *pModelData)
{
	// Set our vertices
	float3* pVertBuf = &pModelData->pVertsArray[0];
	for(DWORD Cpt = 0; Cpt < pModelData->IndexedVerticesCount; Cpt++)
		pMesh->setVert(Cpt,  pVertBuf[Cpt].x, -pVertBuf[Cpt].z, pVertBuf[Cpt].y);

	// Set some pointers...
	UINT *pIndexBuf    = &pModelData->pIndexArray[0];
	BYTE *pEdgesVisBuf = &pModelData->p3dsEdgesVisArray[0];
	// Fill the faces with data
	#ifdef _VS2010_
	DWORD Cpt = 0;
	#endif
	for(Cpt = 0; Cpt < pModelData->VerticesCount; Cpt += 3){
		// Set our vertex index indexes
		pMesh->faces[Cpt/3].setVerts(pIndexBuf[Cpt], pIndexBuf[Cpt+1], pIndexBuf[Cpt+2]);
		// Set the visibility flags of the edges
		pMesh->faces[Cpt/3].setEdgeVisFlags(pEdgesVisBuf[Cpt], pEdgesVisBuf[Cpt+1], pEdgesVisBuf[Cpt+2]);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Model3DImport::FillTexData(Mesh *pMesh, CModelDataStruct *pModelData)
{
	for(int TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++){
		// Is the texture map available
		if(pModelData->AvailArrays[VA_COLOR_ARRAY + TexIndx]){
			// Set support for this texture map active
			pMesh->setMapSupport(TexIndx);
			// Set the number of tex vertices and faces count
			pMesh->setNumMapVerts(TexIndx, pModelData->MapVertsCount[TexIndx]);
			pMesh->setNumMapFaces(TexIndx, pModelData->FacesCount);

			// Create some pointers to our data to fill...
			TVFace *mpFaces = pMesh->mapFaces(TexIndx);
			UVVert *mpVerts = pMesh->mapVerts(TexIndx);

			// Fill the data in those pointers
			memcpy(mpFaces, &pModelData->pTexFacesArray[TexIndx][0], pModelData->VerticesCount * sizeof(UINT));
			memcpy(mpVerts, &pModelData->pTexVertsArray[TexIndx][0], pModelData->MapVertsCount[TexIndx] * sizeof(float3));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Model3DImport::ReadOldFile(Mesh *pMesh, char *pNodeName)
{
	int Cpt = 0;
	int FacesCount = 0;
	int VertexCount = 0;

	// Read the options
	BYTE Opts = 0;
	fread(&Opts, 1, sizeof(BYTE), InF);
	COldOptions Options;
	Options.TexVerts    = ReadBit(&Opts, 1, sizeof(BYTE));
	Options.AuxTexVerts = ReadBit(&Opts, 2, sizeof(BYTE));
	Options.Normals     = ReadBit(&Opts, 3, sizeof(BYTE));
	Options.Fog         = ReadBit(&Opts, 4, sizeof(BYTE));

	// Read the object name char by char 'till we find the NULL char...
	BYTE NameLen = 0;
	fread(&NameLen, 1, sizeof(BYTE), InF);
	fread(pNodeName, 1, NameLen, InF);
	
	// Read the useless padding bytes
	DWORD PaddingBytes = 0;
	int NumPaddingBytes = 4 - (NameLen % 4);
	if(NumPaddingBytes > 0 && NumPaddingBytes < 4)
		fread(&PaddingBytes, 1, NumPaddingBytes, InF);

	//////////////////////////////////////////////////////////////////////////

	//Read Faces and vertex count
	fread(&FacesCount, 1, sizeof(FacesCount),  InF);
	fread(&VertexCount,1, sizeof(VertexCount), InF);

	//Get the true FacesCount value(multiplied by 3 by the exporter)
	FacesCount /= 3;

	//Set the number of Faces and vertex count of the vertex array
	pMesh->setNumVerts(VertexCount);
	pMesh->setNumFaces(FacesCount);
	//Set the number of Faces and vertex count of the mesh
	if(Options.TexVerts){
		pMesh->setNumTVerts(VertexCount);
		pMesh->setNumTVFaces(FacesCount);
	}

	//Here we read the faces Index data
	int3 FacesIndexBuf;
	for(Cpt = 0; Cpt < FacesCount; Cpt++){
		//read 3 faces index
		fread(&FacesIndexBuf, 1, sizeof(int3), InF);
		//Set them in our mesh vertex array
		pMesh->faces[Cpt].setVerts(FacesIndexBuf.x, FacesIndexBuf.y, FacesIndexBuf.z);
		//Set them in our mesh tex vertex array, if we have them
		if(Options.TexVerts)
			pMesh->tvFace[Cpt].setTVerts(FacesIndexBuf.x, FacesIndexBuf.y, FacesIndexBuf.z);
		//Set the visibility of the tris edge, so we set all of 'em to 1
		pMesh->faces[Cpt].setEdgeVisFlags(1, 1, 1);
	}

	//Here the vertex one...
	float3 VertBuf;
	for(Cpt = 0; Cpt < VertexCount; Cpt++){
		fread(&VertBuf,1, sizeof(float3), InF);
		pMesh->setVert(Cpt, VertBuf.x, -VertBuf.z, VertBuf.y);
	}

	//Then the texture one...
	if(Options.TexVerts){
		float2 TexVertBuf;
		for(Cpt = 0; Cpt < VertexCount; Cpt++){
			fread(&TexVertBuf,1, sizeof(float2), InF);
			pMesh->setTVert(Cpt, TexVertBuf.x, TexVertBuf.y, 0.0f);
		}
	}

	//Resmooth mesh automatically
	pMesh->AutoSmooth(PI / 4.0f, FALSE, FALSE); 
	//Build the normals
	pMesh->buildNormals();
	pMesh->EnableEdgeList(1);
	//Build the bounding box
	pMesh->buildBoundingBox();
	//Refresh the edge list
	pMesh->InvalidateEdgeList();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Model3DImport::ReadObject(Mesh *pMesh, char *pNodeName, DWORD *pNodeColor, DWORD IsCompressed, WORD Version)
{
	CModelBuffers ModelBuffers;
	CModelDataStruct *pModelData = &ModelBuffers.ModelData;

	// Erase our globals variables
	ZeroMemory(pNodeName, MAX_OBJ_NAME_LEN);
	*pNodeColor = 0;

	///////////////////////////////////////////
	// Load the object header
	///////////////////////////////////////////

	// Read the object name and save it for later use
	fread(&pModelData->ObjName[0], 1, MAX_OBJ_NAME_LEN, InF);
	memcpy(pNodeName, &pModelData->ObjName[0], MAX_OBJ_NAME_LEN);
	// Read the object color
	fread(&pModelData->ObjColor, 1, sizeof(UINT), InF);
	ConvertObjColorFlag(pModelData->ObjColor, pNodeColor);
	// Read the available arrays
	fread(&pModelData->AvailArrays[0], 1,8, InF);
	// Read our buffers length
	fread(&pModelData->IndexedVerticesCount, 1, sizeof(UINT), InF);
	fread(&pModelData->VerticesCount,        1, sizeof(UINT), InF);
	fread(&pModelData->FacesCount,           1, sizeof(UINT), InF);
	for(int TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++)
		fread(&pModelData->MapVertsCount[TexIndx], 1, sizeof(UINT), InF);
	
	///////////////////////////////////////////
	// Allocate memory and load the file data
	///////////////////////////////////////////
	
	// Allocate our buffers memory
	ModelBuffers.AllocateBuffers(pModelData->IndexedVerticesCount, pModelData->FacesCount, &pModelData->MapVertsCount[0]);
	// Read the arrays
	ModelBuffers.ReadFromFile(InF, IsCompressed ? true : false);

	///////////////////////////////////////////
	// Create the object and load it
	///////////////////////////////////////////
	
	//Set the number of vertex and faces
	pMesh->setNumVerts(pModelData->IndexedVerticesCount);
	pMesh->setNumFaces(pModelData->FacesCount);
	
	// Fill the vertices and color/texture maps data
	FillVerticesData(pMesh, pModelData);
	FillTexData(pMesh, pModelData);

	///////////////////////////////////////////
	// Release the object data from memory
	///////////////////////////////////////////
	ModelBuffers.FreeBuffers();

	///////////////////////////////////////////
	// Read the textures
	///////////////////////////////////////////
	if(Version >= VER_2_1){
	
	}

	///////////////////////////////////////////
	// Prepare studio max scene
	///////////////////////////////////////////
	// Resmooth mesh automatically
	pMesh->AutoSmooth(PI / 4.0f, FALSE, FALSE); 
	// Build the normals
	pMesh->buildNormals();
	// Enable the edges visibility
	pMesh->EnableEdgeList(1);
	// Build the bounding box
	pMesh->buildBoundingBox();
	// Refresh the edge list
	pMesh->InvalidateEdgeList();

	///////////////////////////////////////////
	// return success!
	///////////////////////////////////////////
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Model3DImport::DoImport(const TCHAR *filename,ImpInterface *i,
						Interface *gi, BOOL suppressPrompts)
{
	//Open the file
	if((InF = _tfopen(filename, _T("rb"))) == NULL)
		return 0;

	// Read the signature
	if(!ReadSignature()){
		MessageBox(NULL, "Invalid File Signature...", "Error!", 0);
		fclose(InF);
		return 0;	
	}

	WORD VerNum = ReadFileFormatVersion();
	if(VerNum >= VER_2_0){

		DWORD ObjCount     = ReadNumObject();
		DWORD IsCompressed = ReadCompressedFlag();

		// Load the objects one by one
		for(UINT Cpt = 0; Cpt < ObjCount; Cpt++){

			//Create a new TriObject
			TriObject *object = CreateNewTriObject();
			if(!object){
				fclose(InF);
				return FALSE;
			}

			char  szNodeName[MAX_OBJ_NAME_LEN];
			DWORD dwNodeColor;

			//Read in data
			if(ReadObject(&object->GetMesh(), &szNodeName[0], &dwNodeColor, IsCompressed, VerNum)){
				ImpNode *node = i->CreateNode();
				if(!node){
					delete object;
					MessageBox(NULL, "An error as occured!/r/nExiting now...", "Error!", 0);
					fclose(InF);
					return FALSE;
				}

				//Add it to the scene
				Matrix3 tm;
				tm.IdentityMatrix();
				node->Reference(object);
				node->SetTransform(0,tm);
				i->AddNodeToScene(node);
			
				// Set the object name
				node->SetName(szNodeName);
				// Set the object color
				INode *pINode = node->GetINode();
				pINode->SetWireColor(dwNodeColor);
			}
		}
	} else {
		WORD NumObj = 0;
		fseek(InF, strlen(SIGNATURE), SEEK_SET);
		fread(&NumObj, 1, sizeof(WORD), InF);

		// Load the objects one by one
		for(int Cpt = 0; Cpt < NumObj; Cpt++){

			//Create a new TriObject
			TriObject *object = CreateNewTriObject();
			if(!object){
				fclose(InF);
				return FALSE;
			}

			char szNodeName[256];
			ZeroMemory(&szNodeName[0], 256);
			DWORD dwNodeColor = 0xFFFFFFFF;
			
			if(ReadOldFile(&object->GetMesh(), &szNodeName[0])){
				ImpNode *node = i->CreateNode();
				if(!node){
					delete object;
					MessageBox(NULL, "An error as occured!/r/nExiting now...", "Error!", 0);
					fclose(InF);
					return FALSE;
				}

				//Add it to the scene
				Matrix3 tm;
				tm.IdentityMatrix();
				node->Reference(object);
				node->SetTransform(0,tm);
				i->AddNodeToScene(node);
			
				// Set the object name
				node->SetName(szNodeName);
				// Set the object color
				INode *pINode = node->GetINode();
				pINode->SetWireColor(dwNodeColor);
			}
		}
	}

	//Close the file
	fclose(InF);

	//Redraw the view
	i->RedrawViews();

	return 1;
}

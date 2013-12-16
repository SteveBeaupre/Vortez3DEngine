#include "Shader.h"

#ifdef COMPILE_FOR_OPENGL

//-----------------------------------------------------------------------------
// Name : InitFunctionsPointers()
// Desc : Get all the function pointer adress necessary
//-----------------------------------------------------------------------------
void CShader::InitFunctionsPointers()
{
	glUseProgramObjectARB     = (PFNGLUSEPROGRAMOBJECTARBPROC)     wglGetProcAddress("glUseProgramObjectARB");
	glCreateShaderObjectARB   = (PFNGLCREATESHADEROBJECTARBPROC)   wglGetProcAddress("glCreateShaderObjectARB");
	glCreateProgramObjectARB  = (PFNGLCREATEPROGRAMOBJECTARBPROC)  wglGetProcAddress("glCreateProgramObjectARB");
	glDeleteObjectARB         = (PFNGLDELETEOBJECTARBPROC)         wglGetProcAddress("glDeleteObjectARB");
	glShaderSourceARB         = (PFNGLSHADERSOURCEARBPROC)         wglGetProcAddress("glShaderSourceARB");
	glCompileShaderARB        = (PFNGLCOMPILESHADERARBPROC)        wglGetProcAddress("glCompileShaderARB");
	glAttachObjectARB         = (PFNGLATTACHOBJECTARBPROC)         wglGetProcAddress("glAttachObjectARB");
	glLinkProgramARB          = (PFNGLLINKPROGRAMARBPROC)          wglGetProcAddress("glLinkProgramARB");
	glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC) wglGetProcAddress("glGetObjectParameterivARB");
	glGetInfoLogARB           = (PFNGLGETINFOLOGARBPROC)           wglGetProcAddress("glGetInfoLogARB");
	glGetUniformLocationARB   = (PFNGLGETUNIFORMLOCATIONARBPROC)   wglGetProcAddress("glGetUniformLocationARB");
	glUniform1fARB            = (PFNGLUNIFORM1FARBPROC)            wglGetProcAddress("glUniform1fARB");
	glUniform2fARB            = (PFNGLUNIFORM2FARBPROC)            wglGetProcAddress("glUniform2fARB");
	glUniform3fARB            = (PFNGLUNIFORM3FARBPROC)            wglGetProcAddress("glUniform3fARB");
	glUniform4fARB            = (PFNGLUNIFORM4FARBPROC)            wglGetProcAddress("glUniform4fARB");
	glUniform1iARB            = (PFNGLUNIFORM1IARBPROC)            wglGetProcAddress("glUniform1iARB");
	glUniform2iARB            = (PFNGLUNIFORM2IARBPROC)            wglGetProcAddress("glUniform2iARB");
	glUniform3iARB            = (PFNGLUNIFORM3IARBPROC)            wglGetProcAddress("glUniform3iARB");
	glUniform4iARB            = (PFNGLUNIFORM4IARBPROC)            wglGetProcAddress("glUniform4iARB");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name : GetShaderFileSize(FILE *f)
// Desc : Return the size of the current opened file
//-----------------------------------------------------------------------------
UINT CShader::GetShaderFileSize(FILE *f)
{
	fseek(f, 0, SEEK_END);
	UINT RetVal = ftell(f);
	fseek(f, 0, SEEK_SET);

	return RetVal;
}

//-----------------------------------------------------------------------------
// Name : ReadShaderFile(char *fname, GLenum *pShaderID)
// Desc : Read the shader file source code
//-----------------------------------------------------------------------------
bool CShader::ReadShaderFile(char *fname, GLenum *pShaderID)
{
	FILE *f = fopen(fname, "rb");

	if(!f){
		char c[256];
		sprintf(c, "File \"%s\"Not found", fname);
		MessageBox(0, c, "Error!", 0);
		return false;
	}
	
	const UINT FileSize = GetShaderFileSize(f);

	char *pBuf = new char [FileSize+1];
	ZeroMemory(pBuf, FileSize+1);
	fread(pBuf, 1, FileSize, f);
	
	const char *pSourceCode = pBuf;
	glShaderSourceARB(*pShaderID, 1, &pSourceCode, NULL); // pass vshader data

	SAFE_DELETE_ARRAY(pBuf);

	fclose(f);
	return true;
}

//-----------------------------------------------------------------------------
// Name : LoadShaders(char *fname, int Type)
// Desc : Load the shader
//-----------------------------------------------------------------------------
bool CShader::LoadShaders(char *VertexShader_fname, char *FragmentShader_fname)
{
	InitFunctionsPointers();

	if(VertexShader_fname == NULL)
		return false;

	//Check if vertex shader extension is supported
	if(!GLCompatibility.CheckExtension("GL_ARB_vertex_program") || 
	   !GLCompatibility.CheckExtension("GL_ARB_vertex_shader")){
		MessageBox(0, "Vertex Shaders Not Supported...", "Error!", 0);
		return false;
	}

	//Check if fragment shader extension is supported
	if(FragmentShader_fname){
		if(!GLCompatibility.CheckExtension("GL_ARB_fragment_program") || 
		   !GLCompatibility.CheckExtension("GL_ARB_fragment_shader")){
			MessageBox(0, "Fragment Shaders Not Supported...", "Error!", 0);
			return false;
		}
	}

	m_Program = glCreateProgramObjectARB(); //"container for shaders"
	if(!m_Program){
		MessageBox(0, "Unable to create shader program object...", "Error!", 0);
		return false;
	}


	{
		// Vertex shader
		m_vShaderID = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB); // "vertex shader"
		if(!m_vShaderID){
			MessageBox(0, "Unable to create Vertex shader object...", "Error!", 0);
			return false;
		}

		//glShaderSourceARB(m_vShaderID, vshaderCode, vshaderLen); // pass vshader data
		if(!ReadShaderFile(VertexShader_fname, &m_vShaderID)){
			MessageBox(0, "Unable to Read Vertex shader object...", "Error!", 0);
			return false;
		}

		glCompileShaderARB(m_vShaderID); // compile vshader
		
		int success = 0;
		glGetObjectParameterivARB(m_vShaderID, GL_OBJECT_COMPILE_STATUS_ARB, &success);

		if(!success){
			const int MaxInfoLogSize = 4096;
			char infoLog[MaxInfoLogSize];
			
			glGetInfoLogARB(m_vShaderID, MaxInfoLogSize, NULL, infoLog);

			char c[MaxInfoLogSize+256];
			sprintf(c, "%s\r\n\r\n%s", "Vertex shader compilation failed...", infoLog);
			MessageBox(0, c, "Error!", 0);
			return false;
		}

		//glAttachObjectARB(m_Program, m_vShaderID);
	}

	if(FragmentShader_fname != NULL){
		// Optional Fragment (Pixel) shader
		m_fShaderID = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB); // "fragment shader"
		if(!m_fShaderID){
			MessageBox(0, "Unable to create Fragment shader object...", "Error!", 0);
			return false;
		}

		//glShaderSourceARB(m_fShaderID, pshaderCode, pshaderLen); // pass pshader data
		if(!ReadShaderFile(FragmentShader_fname, &m_fShaderID)){
			MessageBox(0, "Unable to Read Fragment shader object...", "Error!", 0);
			return false;
		}

		glCompileShaderARB(m_fShaderID); // compile pshader

		int success = 0;
		glGetObjectParameterivARB(m_fShaderID, GL_OBJECT_COMPILE_STATUS_ARB, &success);

		if(!success){
			const int MaxInfoLogSize = 4096;
			char infoLog[MaxInfoLogSize];
			
			glGetInfoLogARB(m_fShaderID, MaxInfoLogSize, NULL, infoLog);

			char c[MaxInfoLogSize+256];
			sprintf(c, "%s\r\n\r\n%s", "Fragment shader compilation failed...", infoLog);
			MessageBox(0, c, "Error!", 0);
			return false;
		}

		glAttachObjectARB(m_Program, m_fShaderID);
		//check for errors <-here->
	}
	glAttachObjectARB(m_Program, m_vShaderID);

	// Optional Initialize Attribute variables <-here->

	glLinkProgramARB(m_Program);

	// Initialize Uniform variables <-here->
	InitShaderDataExt();
	
	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name : FreeShader()
// Desc : Delete shader
//-----------------------------------------------------------------------------
void CShader::FreeShader()
{
	//Destroy the shaders context
	if(m_Program/* && m_IsShaderAvailable*/)
		glDeleteObjectARB(m_Program);
}
#endif
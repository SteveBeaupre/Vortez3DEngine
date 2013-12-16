#include "FBO.h"

#ifdef COMPILE_FOR_OPENGL

CFBO::CFBO()
{
	InitData();
}

CFBO::~CFBO()
{
	ShutdownFBO();
}

void CFBO::InitData()
{
	bFBOSupported  = false;
	bInitialized   = false;
	bEnabled       = false;
	bTextureLoaded = false;
	fboID = NULL;
	rboID = NULL;
	fboTexID = -1;
	TexWidth  = 0;
	TexHeight = 0;
}

bool CFBO::IsFBOSupported()
{
	return bFBOSupported;
}

bool CFBO::IsInitialized()
{
	return bInitialized;
}

bool CFBO::IsEnabled()
{
	return bEnabled;
}

bool CFBO::IsTextureLoaded()
{
	return bTextureLoaded;
}

UINT CFBO::GetFBOTexID()
{
	return fboTexID;
}

UINT CFBO::GetTexWidth()
{
	return TexWidth;
}

UINT CFBO::GetTexHeight()
{
	return TexHeight;
}

float CFBO::GetTexWidthf()
{
	return (float)TexWidth;
}

float CFBO::GetTexHeightf()
{
	return (float)TexHeight;
}


bool CFBO::LoadExtentions()
{
	if(!GLCompatibility.CheckExtension("GL_EXT_framebuffer_object") || !GLCompatibility.CheckExtension("GL_EXT_packed_depth_stencil"))
		return false;

	glBindFramebufferEXT         = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
	glGenFramebuffersEXT         = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
	glDeleteFramebuffersEXT      = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
	glCheckFramebufferStatusEXT  = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
	glFramebufferTexture2DEXT    = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
    glGenRenderbuffersEXT        = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
    glBindRenderbufferEXT        = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
    glRenderbufferStorageEXT     = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
    glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
	glDeleteRenderbuffersEXT     = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
	glGenerateMipmapEXT          = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmapEXT");

	return glBindFramebufferEXT && glGenFramebuffersEXT && glDeleteFramebuffersEXT && glCheckFramebufferStatusEXT && glFramebufferTexture2DEXT && glGenRenderbuffersEXT && glBindRenderbufferEXT && glRenderbufferStorageEXT && glFramebufferRenderbufferEXT && glDeleteRenderbuffersEXT && glGenerateMipmapEXT; 
}

bool CFBO::InitFBO(int w, int h)
{
	ShutdownFBO();

	if(LoadExtentions()){
		TexWidth  = w;
		TexHeight = h;
		// Create a texture 
		glGenTextures(1, &fboTexID);
		glBindTexture(GL_TEXTURE_2D, fboTexID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR/*GL_NEAREST*/);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR/*GL_NEAREST*/);
		glGenerateMipmapEXT(GL_TEXTURE_2D);
		bTextureLoaded = true;
		// Create a framebuffer object
        glGenFramebuffersEXT(1, &fboID);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);
        // Create a renderbuffer object
		glGenRenderbuffersEXT(1, &rboID);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rboID);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL_EXT, w, h);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
        // attach a texture to FBO color attachement point
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, fboTexID, 0);
        // attach a renderbuffer to depth attachment point
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rboID);
        // attach a renderbuffer to stencil attachment point
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rboID);
		// Make sure everything is alright
		bFBOSupported = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT;
		if(!bFBOSupported){
			MessageBox(NULL, "Error setuping the FBO.", "Error!", 0);
		} else {
			bInitialized = true;
		}

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	return bFBOSupported;
}

void CFBO::ShutdownFBO()
{
	if(rboID != 0){
		glDeleteRenderbuffersEXT(1, &rboID);
		fboID = NULL;
	}
	if(fboID != 0){
		glDeleteFramebuffersEXT(1, &fboID);
		fboID = NULL;
	}
	if(fboTexID != -1){
		glDeleteTextures(1, &fboTexID);
		fboTexID = -1;
	}

	InitData();
}

void CFBO::EnableFBO(bool Enabled)
{
	if(bInitialized){
		if(Enabled){
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);
			glPushAttrib(GL_VIEWPORT_BIT);
			glViewport(0,0, TexWidth, TexHeight);
		} else {
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			glPopAttrib();
		}
		bEnabled = Enabled;
	}
}

void CFBO::DrawFBO(int WndWidth, int WndHeight)
{
	if(bInitialized){
		// Clear OpenGL buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_TEXTURE_2D);
		
		glViewport(0, 0, WndWidth, WndHeight);
		glMatrixMode(GL_PROJECTION); 
		glLoadIdentity(); 
		gluOrtho2D(0, WndWidth, 0, WndHeight); 
		glMatrixMode(GL_MODELVIEW); 
		
		glBindTexture(GL_TEXTURE_2D, fboTexID);

		glPushMatrix();
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f,            (float)WndHeight);
			glTexCoord2f(1.0f, 1.0f); glVertex2f((float)WndWidth, (float)WndHeight);
			glTexCoord2f(1.0f, 0.0f); glVertex2f((float)WndWidth, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f,            0.0f);
		glEnd();
		glPopMatrix();
	}
}
#endif
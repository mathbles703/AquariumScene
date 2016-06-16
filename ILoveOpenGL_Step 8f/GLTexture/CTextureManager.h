#ifndef _CTextureManager_HG_
#define _CTextureManager_HG_

// Written by Michael Feeney, Fanshawe College, 2010
// mfeeney@fanshawec.on.ca
// It may be distributed under the terms of the MIT License:
// http://opensource.org/licenses/MIT
// (Also, it would be Supah Cool if I heard this was remotely useful!)
// Use this code at your own risk. It is indented only as a learning aid.

// Loads various textures into memory
// Prepares and loads textures into OpenGL texture units
// Stores texture to textureID look up

#include <string>
#include "CTextureFromBMP.h"
#include <map>
#include <string>
#include "../CError/COpenGLError.h"

class CTextureManager
{
public:
	CTextureManager();
	~CTextureManager();

	void setBasePath( std::string basePath );
//	bool loadTexture( std::string fileName );

	bool Create2DTextureFromBMPFile( std::string textureFileName, bool bGenerateMIPMap );
	//bool CreateCubeTextureFromBMPFiles( std::string cubeMapName, 
	//	                                std::string posX_fileName, std::string negX_fileName, 
	//                                    std::string posY_fileName, std::string negY_fileName, 
	//									std::string posZ_fileName, std::string negZ_fileName, 
	//									bool bGenerateMipmap, bool bIsSeamless );
	// returns zero if no texture
	CTextureFromBMP* getTextureFromTextureName( std::string textureName );

	// Loads the texture bindings based on the order they were loaded
	// (This is OK since you'd be doing a lookup on the texture by file name)
	bool UpdateTextureBindings(void);
	bool GetTexUnitFromName( std::string textureName, GLenum &textureUnit );

	// If returns false, textureNumber is undefined.
	bool GetTextureNumberFromName( std::string textureName, GLuint &textureNumber );
	


	// NEW: February, 2015: Allows the texture manager to create and manage framebuffers
	class CFrameBufferInfo
	{
	public:
		CFrameBufferInfo(): 
			ID(0), width(0.0f), height(0.0f), bHadColourBuffers(false), 
			bHasDepthBuffer(false), depthBufferInternalFormat(0), depthBuffer_texture_ID(0) {};
		//~CFramebufferInfo();
		std::string name;
		GLuint ID;			// From OpenGL
		float width;
		float height;
		//
		bool bHadColourBuffers;
		class CColourBuffer
		{
		public:
			CColourBuffer() : colourBufferAttachment(0), colourBufferInternalFormat(0), colourBuffer_texture_ID(0) {};
			GLenum colourBufferAttachment;
			GLenum colourBufferInternalFormat;
			GLuint colourBuffer_texture_ID;		// From OpenGL
		};
		std::map< GLuint, CColourBuffer > mapColourBuffersByID;
		//
		bool bHasDepthBuffer;		GLenum depthBufferInternalFormat;
		GLuint depthBuffer_texture_ID;	
	};
	// This uses the height and width from the frameBufferInfo, but bases on the rest of the information
	bool createNewOffscreenFrameBuffer(CFrameBufferInfo &frameBufferInfo, GLenum colourBuffersInternalFormat, GLuint numberOfColourBuffers, bool bHasDepthBuffer);
	// This bases the framebuffer information from what's contained in the frameBufferInfo object
	bool createNewOffscreenFrameBuffer(CFrameBufferInfo &frameBufferInfo);
	bool deleteOffscreenFrameBuffer( std::string name );
	bool deleteOffscreenFrameBuffer( GLuint ID );
	GLuint getFrameBufferIDFromName( std::string name );
	bool getFrameBufferInfoFromName( std::string name, CFrameBufferInfo &frameBufferInfo );
	bool getFrameBufferInfoFromID( GLuint ID, CFrameBufferInfo &frameBufferInfo );



	std::string getLastError(void);

	//// Used for offscreen rendering
	//GLuint getNextFreeTextureUnit(void);
	//bool reserveNextFreeTextureUnit(void);

	void ShutDown(void);

private:
	std::map< std::string, CTextureFromBMP* > m_map_TexNameToTexture;
	std::map< GLenum /*textureUnit*/, std::string /*textureName*/ >		m_map_TexUnitToTexName;
	std::map< std::string /*textureName*/, GLenum /*textureUnit*/ >		m_map_TexNameToTexUnit;

	GLuint	m_currentFrameBuffer;		// Zero for default

	//GLuint m_nextTextureUnitOffset;
	//static const GLuint m_BASETEXTURE = GL_TEXTURE0;
	std::string m_basePath;
	std::string m_lastError;
	void m_appendErrorString( std::string nextErrorText );
	void m_appendErrorStringLine( std::string nextErrorTextLine );

	COpenGLError m_GLErrorMotron;
};

#endif





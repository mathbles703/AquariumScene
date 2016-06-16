#ifndef _cMeshManager_HG_
#define _cMeshManager_HG_

//#include <GL/glew.h>
//#include <GL/freeglut.h>
#include "globals.h"		// Also has Vertex struct
#include <string>
#include <vector>
#include <map>
#include "cVertex.h"
#include "cTriangle.h"

class cVBOInfo
{
public:
	//GLuint  BufferIds[3] = { 0 };
	GLuint VBO_ID;		 // BufferIds[0] = VAO (or VBO)
	GLuint vert_buf_ID;	 // BufferIds[1] = vertex buffer ID
	GLuint index_buf_ID; // BufferIds[2] = index buffer ID
	std::string meshFileName;
	unsigned int numberOfTriangles;
};

class cMeshManager
{
public:
	cMeshManager();
	~cMeshManager();

	//// Taken from the main cpp file
	//void LoadPlyFile(std::string filename,
	//	std::vector< cVertex > &vecVerts,
	//	std::vector< cTriangle > &vecTris,
	//	int &numberOfVerts,
	//	int &numberOfTriangle);

	//bool LoadMeshIntoVBO(std::string fileToLoad, 
	//	                 unsigned int &VBO);

	// "Fancier" version that loads more models, WAY faster
	bool LoadPlyIntoVBO( std::string fileToLoad );

	bool LookUpVBOInfoFromModelName( std::string modelName,
		                             cVBOInfo &VBOInfo );

	void ShutDown(void);

private:
	// Look up file name to VBOinfo
	std::map< std::string /*fileName*/,
		      cVBOInfo >  p_mapFileToBVO;

	// Cool method coming... 
};

#endif

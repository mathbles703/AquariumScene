#include "cMeshManager.h"

#include <vector>
#include <fstream>
#include <string>

#include "Ply/CPlyFile5nt.h"
#include "Ply/CStringHelper.h"

cMeshManager::cMeshManager()
{
	return;
}

cMeshManager::~cMeshManager()
{
	// A bunch of clean up code to go here... 
	return;
}

////std::vector< cVertex >	 g_vecVerts;
////std::vector< cTriangle >   g_vecTris;
////int g_numberOfVerts = 0;
////int g_numberOfIndices = 0;
//void cMeshManager::LoadPlyFile(std::string filename,
//	std::vector< cVertex > &vecVerts,
//	std::vector< cTriangle > &vecTris,
//	int &numberOfVerts,
//	int &numberOfTriangles)
//{
//	// Open file
//	std::ifstream thePly(filename.c_str());
//	if (!thePly.is_open())
//	{	// Everything is lost, forever lost... 
//		return;
//	}
//
//	// Read until we get to the word "vertex"
//	std::string temp;
//	while (thePly >> temp)
//	{	// The word vertex? 
//		if (temp == "vertex")
//		{	// Yup
//			break;
//		}
//	}
//	// The next number is the number of vertices, yo
//	thePly >> numberOfVerts;
//
//	// Read until we get to "face"
//	while (thePly >> temp)
//	{	// The word face? 
//		if (temp == "face")
//		{	// Yup
//			break;
//		}
//	}
//	// The next number is the number of indices, people
//	thePly >> numberOfTriangles;
//
//	// Read until "end_header"
//	while (thePly >> temp)
//	{	// The word end_header? 
//		if (temp == "end_header")
//		{	// Yup
//			break;
//		}
//	}
//
//	// Read vertices
//	for (int index = 0; index != numberOfVerts; index++)
//	{
//		cVertex tempVert;
//		thePly >> tempVert.x >> tempVert.y >> tempVert.z;
//		// Add it to the vector
//		vecVerts.push_back(tempVert);
//	}
//
//	// Read triangles (aka "faces" or "elements")
//	for (int index = 0; index != numberOfTriangles; index++)
//	{
//		cTriangle tempTri;		// 3 13 14 23 
//		int discartThis;
//		thePly >> discartThis >> tempTri.v0 >> tempTri.v1 >> tempTri.v2;
//		vecTris.push_back(tempTri);
//	}
//
//	// Copy data into GPU Vertex Buffer Object.
//	return;
//}

// "Fancier" version that loads more models, WAY faster
bool cMeshManager::LoadPlyIntoVBO( std::string fileToLoad )
{
	CPlyFile5nt plyFile;
	std::wstring error;
	if (!plyFile.OpenPLYFile2( CStringHelper::getInstance( )->ASCIIToUnicodeQnD( fileToLoad ), error ))
	{
		return false;
	}

	if ( ! plyFile.bHasNormalsInFile() )
	{
		plyFile.normalizeTheModelBaby();
	}
	// 
	plyFile.normlizeExistingNomrals();

	// Are there any texture coordinates? 
	if ( ! plyFile.bHasTextureCoordinatesInFile() )
	{	// Calculate some "good enough for rock-n-roll" coordinates (spherical)
		plyFile.GenTextureCoordsSpherical( CPlyFile5nt::POSITIVE_X, CPlyFile5nt::POSITIVE_Y, 
										   false,	// Base the spherical projection on the normal
										   1.0f,	// Texture coordinates are from 0.0 to 1.0 at 1.0x scale
										   false );	// do it "fast" (kind of pointless, really)
	}


	// Do some magic
	Vertex_xyz_n_RGB_UVx2* pVerts = new Vertex_xyz_n_RGB_UVx2[ plyFile.GetNumberOfVerticies() ];
	// Copy the vertices from the nice vector to the 
	// raw, evil, array
	PlyVertex tempPlyVert;
	for (int index = 0; index != plyFile.GetNumberOfVerticies(); index++)
	{
		tempPlyVert = plyFile.getVertex_at(index);

		pVerts[index].Position[0] = tempPlyVert.xyz.x;
		pVerts[index].Position[1] = tempPlyVert.xyz.y;
		pVerts[index].Position[2] = tempPlyVert.xyz.z;
		pVerts[index].Position[3] = 1.0f;		// w coordinate

		pVerts[index].Normal[0] = tempPlyVert.nx;
		pVerts[index].Normal[1] = tempPlyVert.ny;
		pVerts[index].Normal[2] = tempPlyVert.nz;
		pVerts[index].Normal[3] = 1.0f;			// (1.0 unless...)

		pVerts[index].RGBA[0] = tempPlyVert.red;
		pVerts[index].RGBA[1] = tempPlyVert.green;
		pVerts[index].RGBA[2] = tempPlyVert.blue;
		pVerts[index].RGBA[3] = tempPlyVert.alpha;			// (1.0 unless...)

		pVerts[index].UVx2[0] = tempPlyVert.tex0u;			// s
		pVerts[index].UVx2[1] = tempPlyVert.tex0v;			// t
		pVerts[index].UVx2[2] = tempPlyVert.tex1u;
		pVerts[index].UVx2[3] = tempPlyVert.tex1v;			// (1.0 unless...)

	}

	unsigned int numIndices = plyFile.GetNumberOfElements() * 3;
	GLuint* pIndices = new GLuint[numIndices];

	int triIndex = 0;
	PlyElement tempPlyElement;
	for (; triIndex != plyFile.GetNumberOfElements();
		triIndex++)
	{
		tempPlyElement = plyFile.getElement_at(triIndex);
		pIndices[triIndex * 3 + 0] = tempPlyElement.vertex_index_1;
		pIndices[triIndex * 3 + 1] = tempPlyElement.vertex_index_2;
		pIndices[triIndex * 3 + 2] = tempPlyElement.vertex_index_3;
	}

	cVBOInfo tempVBOInfo;

	//glGenVertexArrays(1, &BufferIds[0]);
	glGenVertexArrays(1, &(tempVBOInfo.VBO_ID) );
	ExitOnGLError("ERROR: Could not generate the VAO");	// AKA VBO
	//glBindVertexArray(BufferIds[0]);
	glBindVertexArray(tempVBOInfo.VBO_ID);
	ExitOnGLError("ERROR: Could not bind the VAO");

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	ExitOnGLError("ERROR: Could not enable vertex attributes");

	// GLuint BufferIds[3];   0,1,2
	//glGenBuffers(2, & BufferIds[1]);
	glGenBuffers(1, &(tempVBOInfo.vert_buf_ID));
	glGenBuffers(1, &(tempVBOInfo.index_buf_ID));

	ExitOnGLError("ERROR: Could not generate the buffer objects");

	//glBindBuffer(GL_ARRAY_BUFFER, BufferIds[1]);
	glBindBuffer(GL_ARRAY_BUFFER, tempVBOInfo.vert_buf_ID );
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(Vertex_xyz_n_RGB_UVx2) * plyFile.GetNumberOfVerticies(),	// sizeof(VERTICES), 
		pVerts,								// VERTICES,
		GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the VBO to the VAO");

//	Vertex tempVert;
	unsigned int bytesInOneVertex = sizeof(Vertex_xyz_n_RGB_UVx2);
	unsigned int byteOffsetToPosition = offsetof( Vertex_xyz_n_RGB_UVx2, Position );
	unsigned int byteOffsetToNormal = offsetof( Vertex_xyz_n_RGB_UVx2, Normal );
	unsigned int byteOffsetToRGBA = offsetof( Vertex_xyz_n_RGB_UVx2, RGBA );
	unsigned int byteOffsetToUVCoords = offsetof( Vertex_xyz_n_RGB_UVx2, UVx2 );


	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		bytesInOneVertex,	
		(GLvoid*)byteOffsetToPosition);	// At start of each vertex

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
		bytesInOneVertex,	// sizeof(VERTICES[0]), 
		(GLvoid*)byteOffsetToNormal);		//sizeof(Vertex_xyz_n_RGB_UVx2::Position));	// (GLvoid*)sizeof(VERTICES[0].Position));

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,
		bytesInOneVertex,	
		(GLvoid*)byteOffsetToRGBA);		//sizeof(Vertex_xyz_n_RGB_UVx2::Normal));	

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE,
		bytesInOneVertex,	
		(GLvoid*)byteOffsetToUVCoords);		//sizeof(Vertex_xyz_n_RGB_UVx2::RGBA));	

	ExitOnGLError("ERROR: Could not set VAO attributes");

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferIds[2]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempVBOInfo.index_buf_ID);

	unsigned int sizeOfIndexArray =
		sizeof(GLuint) * plyFile.GetNumberOfElements() * 3;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeOfIndexArray,	// sizeof(INDICES),
		(GLvoid*)pIndices,			// INDICES,
		GL_STATIC_DRAW);

	ExitOnGLError("ERROR: Could not bind the IBO to the VAO");

	glBindVertexArray(0);

	// Clean up
	delete[] pVerts;			// note odd syntax
	delete[] pIndices;		// note odd syntax

	tempVBOInfo.meshFileName = fileToLoad;
	tempVBOInfo.numberOfTriangles = plyFile.GetNumberOfElements();
	this->p_mapFileToBVO[tempVBOInfo.meshFileName] = tempVBOInfo;


	return true;
}

void cMeshManager::ShutDown(void)
{
	// Lines from the original code...
	//  glDeleteBuffers(2, &BufferIds[1]);
	//  glDeleteVertexArrays(1, &BufferIds[0]);	
	
	// Go through all the VBAs and delete them 

	for ( std::map< std::string /*fileName*/, cVBOInfo >::iterator itVBA = this->p_mapFileToBVO.begin();
		  itVBA != this->p_mapFileToBVO.end(); itVBA++ )
	{
		glDeleteBuffers( 1, &(itVBA->second.index_buf_ID) );
		glDeleteBuffers( 1, &(itVBA->second.vert_buf_ID) );

		glDeleteVertexArrays( 1, &(itVBA->second.VBO_ID) );
	}

	return;
}


//bool cMeshManager::LoadMeshIntoVBO(std::string fileToLoad,
//	                               unsigned int &VBO)
//{
//	//LoadPlyFile("tie_Unit_BBox.ply");
//
//	std::vector< cVertex > vecVerts;
//	std::vector< cTriangle > vecTriangles;
//	int numberOfVertices = 0;
//	int numberOfTriangles = 0;
//
//	this->LoadPlyFile(fileToLoad, vecVerts, vecTriangles,
//		              numberOfVertices, numberOfTriangles);
//
//
//	//const Vertex VERTICES[8] =
//	//  { { -0.5f, -.5f,  .5f, 1 }, { 1, 1, 1, 1 } },
//
//	//const Vertex VERTICES[8] =  
//	//{		                         // GLUT_RGBA
//	//	// 1 0 1 (1 = alpha)
//	//  { { -0.5f, -.5f,  .5f, 1 }, { 1, 1, 1, 1 } },
//	//  { { -.5f,  .5f,  .5f, 1 },  { 1, 1, 1, 1 } },
//	//  { {  0.5f,  .5f,  .5f, 1 }, { 1, 1, 1, 1 } },
//	//  { {  .5f, -.5f,  .5f, 1 },  { 1, 1, 1, 1 } },
//	//  { { -.5f, -.5f, -.5f, 1 },  { 1, 1, 1, 1 } },
//	//  { { -.5f,  .5f, -.5f, 1 },  { 1, 1, 1, 1 } },
//	//  { {  .5f,  .5f, -.5f, 1 },  { 1, 1, 1, 1 } },
//	//  { {  .5f, -.5f, -.5f, 1 },  { 1, 1, 1, 1 } }
//	//};
//	Vertex* pVerts = new Vertex[numberOfVertices];
//	// Copy the vertices from the nice vector to the 
//	// raw, evil, array
//	for (int index = 0; index != numberOfVertices; index++)
//	{
//		pVerts[index].Position[0] = vecVerts[index].x;
//		pVerts[index].Position[1] = vecVerts[index].y;
//		pVerts[index].Position[2] = vecVerts[index].z;
//		pVerts[index].Position[3] = 1.0f;		// w coordinate
//
//		pVerts[index].Color[0] = 1.0f;	// red
//		pVerts[index].Color[1] = 1.0f;	// green
//		pVerts[index].Color[2] = 1.0f;	// green
//		pVerts[index].Color[3] = 1.0f;	// alpha (1.0 unless...)
//
//	}
//
//	//  const GLuint INDICES[36] = 0,2,1,  0,3,2,  and so on... 
//
//	//{
//	//  0,2,1,  0,3,2,
//	//  4,3,0,  4,7,3,
//	//  4,1,5,  4,0,1,
//	//  3,6,2,  3,7,6,
//	//  1,6,5,  1,2,6,
//	//  7,5,6,  7,4,5
//	//};
//	unsigned int numIndices = numberOfTriangles * 3;
//	GLuint* pIndices = new GLuint[numIndices];
//
//	int triIndex = 0;
//	for (; triIndex != numberOfTriangles;
//		triIndex++)
//	{
//		cTriangle tempTri = vecTriangles[triIndex];
//
//		pIndices[triIndex * 3 + 0] = tempTri.v0;
//		pIndices[triIndex * 3 + 1] = tempTri.v1;
//		pIndices[triIndex * 3 + 2] = tempTri.v2;
//	}
//
//	//ShaderIds[0] = glCreateProgram();
//	//ExitOnGLError("ERROR: Could not create the shader program");
//	//{
//	//	ShaderIds[1] = LoadShader("SimpleShader.fragment.glsl", GL_FRAGMENT_SHADER);
//	//	ShaderIds[2] = LoadShader("SimpleShader.vertex.glsl", GL_VERTEX_SHADER);
//	//	glAttachShader(ShaderIds[0], ShaderIds[1]);
//	//	glAttachShader(ShaderIds[0], ShaderIds[2]);
//	//}
//	//glLinkProgram(ShaderIds[0]);
//	//ExitOnGLError("ERROR: Could not link the shader program");
////
//	//ModelMatrixUniformLocation
//	//	= glGetUniformLocation(ShaderIds[0], "ModelMatrix");
////
//	//ViewMatrixUniformLocation
//	//	= glGetUniformLocation(ShaderIds[0], "ViewMatrix");
////
//	//ProjectionMatrixUniformLocation
//	//	= glGetUniformLocation(ShaderIds[0], "ProjectionMatrix");
////
//	//ExitOnGLError("ERROR: Could not get shader uniform locations");
//
//	cVBOInfo tempVBOInfo;
//
//	//glGenVertexArrays(1, &BufferIds[0]);
//	glGenVertexArrays(1, &(tempVBOInfo.VBO_ID) );
//	ExitOnGLError("ERROR: Could not generate the VAO");	// AKA VBO
//	//glBindVertexArray(BufferIds[0]);
//	glBindVertexArray(tempVBOInfo.VBO_ID);
//	ExitOnGLError("ERROR: Could not bind the VAO");
//
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//	ExitOnGLError("ERROR: Could not enable vertex attributes");
//
//	// GLuint BufferIds[3];   0,1,2
//	//glGenBuffers(2, & BufferIds[1]);
//	glGenBuffers(1, &(tempVBOInfo.vert_buf_ID));
//	glGenBuffers(1, &(tempVBOInfo.index_buf_ID));
//
//	ExitOnGLError("ERROR: Could not generate the buffer objects");
//
//	//glBindBuffer(GL_ARRAY_BUFFER, BufferIds[1]);
//	glBindBuffer(GL_ARRAY_BUFFER, tempVBOInfo.vert_buf_ID );
//	glBufferData(GL_ARRAY_BUFFER,
//		sizeof(Vertex) * numberOfVertices,	// sizeof(VERTICES), 
//		pVerts,								// VERTICES,
//		GL_STATIC_DRAW);
//	ExitOnGLError("ERROR: Could not bind the VBO to the VAO");
//
////	Vertex tempVert;
//
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
//	                      sizeof(Vertex),			// sizeof(tempVert),	//  sizeof(VERTICES[0]),
//	                      (GLvoid*)0);	// At start of each vertex
//
//	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
//	                      sizeof(Vertex),	// sizeof(tempVert),	// sizeof(VERTICES[0]), 
//	                      (GLvoid*)sizeof(Vertex::Position));	//(GLvoid*)sizeof(tempVert.Position));	// (GLvoid*)sizeof(VERTICES[0].Position));
//
//	ExitOnGLError("ERROR: Could not set VAO attributes");
//
//	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferIds[2]);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempVBOInfo.index_buf_ID);
//
//	unsigned int sizeOfIndexArray =
//		sizeof(GLuint) * numberOfTriangles * 3;
//
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//		sizeOfIndexArray,	// sizeof(INDICES),
//		(GLvoid*)pIndices,			// INDICES,
//		GL_STATIC_DRAW);
//
//	ExitOnGLError("ERROR: Could not bind the IBO to the VAO");
//
//	glBindVertexArray(0);
//
//	// Clean up
//	delete[] pVerts;			// note odd syntax
//	delete[] pIndices;		// note odd syntax
//
//							// Assume we are good to go...
////	VBO = BufferIds[0]; // "return" the VBO (VAO) ID
//
//	tempVBOInfo.meshFileName = fileToLoad;
//	tempVBOInfo.numberOfTriangles = numberOfTriangles;
//	VBO = tempVBOInfo.VBO_ID;
//
//	this->p_mapFileToBVO[tempVBOInfo.meshFileName] = tempVBOInfo;
//
//
//	return true;
//}


bool cMeshManager::LookUpVBOInfoFromModelName(
	std::string modelName,
	cVBOInfo &VBOInfo)
{
	  p_mapFileToBVO;

	// Find the model
	  std::map< std::string /*fileName*/, cVBOInfo >::iterator itVBO
		                  = this->p_mapFileToBVO.find(modelName);
	// Found it? 
	  if ( itVBO == this->p_mapFileToBVO.end() )
	  {		// 
		  return false;
	  }
	  // Did find it
	  VBOInfo = itVBO->second;

	return true;
}
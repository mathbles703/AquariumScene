#include "globals.h"
#define WINDOW_TITLE_PREFIX "Week 5, Day 1 (ligthing)"

#include <iostream>
#include <sstream>

// To load the ply file
#include <string>
#include <fstream>
#include <vector>
#include "cVertex.h"
#include "cTriangle.h"
#include <sstream>		// Why isn't it stringstream.... really?
#include <iomanip>

//void LoadPlyFile(std::string filename, 
//	             std::vector< cVertex > &vecVerts,
//	             std::vector< cTriangle > &vecTris, 
//	             int &numberOfVerts, 
//	             int &numberOfTriangle );

//std::vector< cVertex >   g_vecVerts;
//std::vector< cTriangle > g_vecTris;
//int g_numberOfVerts = 0;
//int g_numberOfTriangles = 0;

#include "cGameObject.h"
#include "cMeshManager.h"
#include "CShaderManager/CGLShaderManager.h"	// Note: it's "C" here

#include <sstream>

cMeshManager* g_pTheMeshManager = 0;

IGLShaderManager* g_pTheShaderManager = 0;
CTextureManager* g_pTheTextureManager = 0;

cGameObject* g_pDebugBall = 0;

bool g_bDebugLights = false;


int
  CurrentWidth = 800,
  CurrentHeight = 600,
  WindowHandle = 0;

unsigned FrameCount = 0;

GLint  ProjectionMatrixUniformLocation = 0;
GLint  ViewMatrixUniformLocation = 0;
GLint  ModelMatrixUniformLocation = 0;
GLint  ModelMatrixRotationOnlyUniformLocation = 0;

std::vector<cLightDesc> g_vecLights;




// Fragment shader uniforms...
// For directly setting the ambient and diffuse (if NOT using textures)
GLint UniLoc_MaterialAmbient_RGB = 0;
GLint UniLoc_MaterialDiffuse_RGB = 0;
GLint UniLoc_MaterialSpecular = 0;
GLint UniLoc_MaterialShininess = 0;
GLint UniLoc_eye = 0;
GLint UniLoc_debugColour = 0;
GLint UniLoc_bUseDebugColour = 0;
GLint UniLoc_bUseVertexRGBAColours = 0;
GLint UniLoc_bUseTextureMaterials = 0;	
GLint UniLoc_bUseTexturesOnly = 0;

GLint UniLoc_myAlphaAllObject = 0;
GLint UniLoc_bAlphaForEntireObject = 0;

GLint UniLoc_bUseDiscardMask = 0;

// This is taken from the shader... (the fact we have 8 samplers)
// NOTE: we are using an array here, but you CAN'T have sampler arrays
//	in this way inside the shader. There are things called "texture arrays",
//	but they are NOT the same thing at all. 
static const unsigned int NUMBEROF2DSAMPLERS = 12;
GLint UniLoc_texSampler2D[NUMBEROF2DSAMPLERS] = {0};
GLint UniLoc_texMix[NUMBEROF2DSAMPLERS] = {0};

//
//GLint UniLoc_Light_0_position = 0;
//GLint UniLoc_Light_0_ambient = 0;
//GLint UniLoc_Light_0_diffuse = 0;
//GLint UniLoc_Light_0_specular = 0;
//GLint UniLoc_Light_0_attenConst = 0;
//GLint UniLoc_Light_0_attenLinear = 0;
//GLint UniLoc_Light_0_attenQuad = 0;
//
//GLint UniLoc_Light_1_position = 0;
//GLint UniLoc_Light_1_ambient = 0;
//GLint UniLoc_Light_1_diffuse = 0;
//GLint UniLoc_Light_1_specular = 0;
//GLint UniLoc_Light_1_attenConst = 0;
//GLint UniLoc_Light_1_attenLinear = 0;
//GLint UniLoc_Light_1_attenQuad = 0;

//GLuint  BufferIds[3] = { 0 };
        // BufferIds[0] = VAO (or VBO)
        // BufferIds[1] = vertex buffer ID
		// BufferIds[2] = index buffer ID
//GLuint  ShaderIds[3] = { 0 };

//Matrix
//  ProjectionMatrix,
//  ViewMatrix,
//  ModelMatrix;

glm::mat4 matProjection;
glm::mat4 matView;			// "camera"
glm::mat4 matWorld;			// "model"

float CubeRotation = 0;
clock_t LastTime = 0;

void Initialize(int, char*[]);
void InitWindow(int, char*[]);
void ResizeFunction(int, int);
void RenderFunction(void);
void TimerFunction(int);
void IdleFunction(void);

void SetupShader(void);

//void CreateCube(void);
//bool LoadMeshIntoVBO(std::string fileToLoad, unsigned int &VBO);

//void DestroyCube(void);
void ShutErDownPeople(void);

// Add keyboard and mouse callbacks
void myKeyboardCallback( unsigned char keyCode,
	                     int x, int y );
// for arrow, PigUp, PigDown, F1, etc.
void mySpecialKeyboardCallback(int key, int x, int y);

void myMouseCallback();		// Look into this if you like

//void DrawCube(void);
void DrawObject(cGameObject* pGO);

void CreateTheObjects(void);
void SetUpInitialLightValues(void);

int g_selectedLightIndex = 0;

int main(int argc, char* argv[])
{
	std::cout << "Preparing OpenGL..." << std::endl;
  Initialize(argc, argv);

  std::cout << "Loading objects..." << std::endl;
  CreateTheObjects();

  std::cout << "Setting up initial light values..." << std::endl;
  SetUpInitialLightValues();


  ::g_cam_at = glm::vec3( 2.0f, 0.0f, 0.0f );
  ::g_cam_eye = glm::vec3( 0.0f, 2.0f, 20.0f );

  g_AniTimer.Reset( );
  g_AniTimer.Start( );

  std::cout << "Boom!" << std::endl;
  glutMainLoop();

  exit(EXIT_SUCCESS);
}

void Initialize(int argc, char* argv[])
{
  GLenum GlewInitResult;

  InitWindow(argc, argv);
  
  glewExperimental = GL_TRUE;
  GlewInitResult = glewInit();

  if (GLEW_OK != GlewInitResult) {
    fprintf(
      stderr,
      "ERROR: %s\n",
      glewGetErrorString(GlewInitResult)
    );
    exit(EXIT_FAILURE);
  }
  
  fprintf(
    stdout,
    "INFO: OpenGL Version: %s\n",
    glGetString(GL_VERSION)
  );

  glGetError();
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  ExitOnGLError("ERROR: Could not set OpenGL depth testing options");

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  ExitOnGLError("ERROR: Could not set OpenGL culling options");

//  ModelMatrix = IDENTITY_MATRIX;
//  ProjectionMatrix = IDENTITY_MATRIX;
//  ViewMatrix = IDENTITY_MATRIX;
//  TranslateMatrix(&ViewMatrix, 0, 0, -2);


  SetupShader();

  ::g_pTheMeshManager = new cMeshManager();

//  CreateCube();
  //unsigned int VBO_ID = 0;
  //if ( !::g_pTheMeshManager->LoadMeshIntoVBO("tie_Unit_BBox.ply", VBO_ID) )
  //{
	 // std::cout << "Oh no." << std::endl;
	 // exit(-1);
  //}
  //// Assume we are loading these just fine, thank you very much...
  //::g_pTheMeshManager->LoadMeshIntoVBO("bun_zipper_res3_1UnitBB.ply", VBO_ID);
  //::g_pTheMeshManager->LoadMeshIntoVBO("mig29_xyz.ply", VBO_ID);
  //::g_pTheMeshManager->LoadMeshIntoVBO("Seafloor2.ply", VBO_ID);
  //// Do more magic


	// Moved to "CreateTheObjects()"

  return;
}

void InitWindow(int argc, char* argv[])
{
  glutInit(&argc, argv);
  
  glutInitContextVersion(4, 0);
  glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
  glutInitContextProfile(GLUT_CORE_PROFILE);

  glutSetOption(
    GLUT_ACTION_ON_WINDOW_CLOSE,
    GLUT_ACTION_GLUTMAINLOOP_RETURNS
  );
  
  glutInitWindowSize(CurrentWidth, CurrentHeight);

  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

  WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);

  if(WindowHandle < 1) {
    fprintf(
      stderr,
      "ERROR: Could not create a new rendering window.\n"
    );
    exit(EXIT_FAILURE);
  }
  
  glutReshapeFunc(ResizeFunction);
  glutDisplayFunc(RenderFunction);
  glutIdleFunc(IdleFunction);
  glutTimerFunc(0, TimerFunction, 0);
  glutCloseFunc(ShutErDownPeople);		//  glutCloseFunc(DestroyCube);
  // Register the keyboard callback
  glutKeyboardFunc(myKeyboardCallback);
  //glutKeyboardUpFunc()
  glutSpecialFunc(mySpecialKeyboardCallback);
}

void ResizeFunction(int Width, int Height)
{
  CurrentWidth = Width;
  CurrentHeight = Height;
  glViewport(0, 0, CurrentWidth, CurrentHeight);
  //ProjectionMatrix =
  //  CreateProjectionMatrix(
  //    60,
  //    (float)CurrentWidth / CurrentHeight,
  //    1.0f,
  //    100.0f
  //  );
  matProjection = glm::perspective(
	              // 60.0f,				// <-- not this
	              glm::radians(60.0f),  // <-- this instead
                  (float)CurrentWidth / CurrentHeight,
                  0.1f,		// Near 
                  10000.0f    // Far
   );	              

//  glUseProgram(ShaderIds[0]);
  ::g_pTheShaderManager->UseShaderProgram("basicShader");

//  glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, 
//	  ProjectionMatrix.m);
  glUniformMatrix4fv(ProjectionMatrixUniformLocation, 
	                 1, GL_FALSE, 
	                 glm::value_ptr(matProjection) );
//  glUseProgram(0);
  ::g_pTheShaderManager->UseShaderProgram(0);
}

// Will move this later... 
//cGameObject* pTheOneGO = 0;

void SetLightUniforms(void)
{
	  	for ( int index = 0; index != NUMBEROFLIGHTS; index++ )
	{
		glUniform4f( ::g_vecLights[index].UniLoc_position, 
					 ::g_vecLights[index].position.x, 
					 ::g_vecLights[index].position.y, 
					 ::g_vecLights[index].position.z, 
					 ::g_vecLights[index].position.w );
		glUniform4f( ::g_vecLights[index].UniLoc_diffuse, 
					 ::g_vecLights[index].diffuse.x, 
					 ::g_vecLights[index].diffuse.y, 
					 ::g_vecLights[index].diffuse.z, 
					 ::g_vecLights[index].diffuse.w );
		glUniform4f( ::g_vecLights[index].UniLoc_ambient, 
					 ::g_vecLights[index].ambient.x, 
					 ::g_vecLights[index].ambient.y, 
					 ::g_vecLights[index].ambient.z, 
					 ::g_vecLights[index].ambient.w );
		glUniform4f( ::g_vecLights[index].UniLoc_specular, 
					 ::g_vecLights[index].specular.x, 
					 ::g_vecLights[index].specular.y, 
					 ::g_vecLights[index].specular.z, 
					 ::g_vecLights[index].specular.w );
		glUniform1f( ::g_vecLights[index].UniLoc_attenConst, ::g_vecLights[index].attenConst);
		glUniform1f( ::g_vecLights[index].UniLoc_attenLinear, ::g_vecLights[index].attenLinear);
		glUniform1f( ::g_vecLights[index].UniLoc_attenQuad, ::g_vecLights[index].attenQuad);

		glUniform1f( ::g_vecLights[index].UniLoc_type, static_cast<float>(::g_vecLights[index].lightType) );
		
		glUniform4f( ::g_vecLights[index].UniLoc_direction, 
					 ::g_vecLights[index].direction.x, ::g_vecLights[index].direction.y,
					 ::g_vecLights[index].direction.z, ::g_vecLights[index].direction.w );
		glUniform1f( ::g_vecLights[index].UniLoc_anglePenumbraStart, ::g_vecLights[index].anglePenumbraStart );
		glUniform1f( ::g_vecLights[index].UniLoc_anglePenumbraEnd, ::g_vecLights[index].anglePenumbraEnd );
		// ... and so on.. 
	}

	ExitOnGLError("ERROR in SetUpLightUniforms()");

	return;
}

// Here's a handy function that will set the texture and binding attrubutes at once
bool SetTextureBinding( std::string texture, GLenum textureUnit, GLint samplerNumber )
{
	GLuint textureNum = 0;
	if ( ! ::g_pTheTextureManager->GetTextureNumberFromName(texture, textureNum) )
	{	// Can't find that texture in the texture manager.
		return false;
	}
	// Note that we are subtracting the define GL_TEXTURE0;
	// if you look at the defines, they are in order. Convenient, eh?
	glActiveTexture( textureUnit );					// GL_TEXTURE0, etc.
	glBindTexture( GL_TEXTURE_2D, textureNum );		// Number from glGenTexture()
	glUniform1i( UniLoc_texSampler2D[samplerNumber], textureUnit - GL_TEXTURE0 );	// 0, 1, etc.	

	//ExitOnGLError("ERROR: AssignTextureUnitSimple()");

	return true;
}

bool AssignTextureUnitsSimple(void)
{
	// This simply sets the texture units in the shader based on how they were 
	//	loaded by the texture manager.

//	::g_pTheTextureManager->UpdateTextureBindings();

	// The textures are loaded into texture "units", but they are not "bound" 
	//  to a "binding point" or to a "sampler unit" in the shader
	// 
	// So you have to do two things:
	// * Assign the texture unit the "bind point" in the GPU
	// * Assign this "bind point" to a particular "sampler unit" in the shader
	// 
	// Serious "gotcha": There are a couple ways to assign texture units, 
	//  one that has been around for a long time (and is more common), and a newer
	//  way (OpenGL 4.5 and later). The difference is the number of texture units 
	//  that are available. Pre OpenGL 4.5 had at least 16 or 32 texture unit (and often 
	//  at least 80), that have pre-defined constants: GL_TEXTURE0 through GL_TEXTURE31.
	//  (and, in fact, you can go up to 80 here, possibly)
	// Keep in mind that this is NOT the total number of textures you can load, but 
	//	simply the NUMBER OF TEXTURES YOU CAN ASSIGN TO EACH SHADER AT ANY TIME. 
	// But another way, you could load 1000s of textures (until your GPU memory is full), 
	//  but for each shader, you could only assign 16-32 AT THE SAME TIME. 
	// Huh? Bottom line: that's a TON of textures - even with a lower end (like inexpensive
	//  circa 2010 GPU), you can sample ("multi-sample") 16 textures at the same time, for 
	//  the SAME PIXEL FRAGMENT. 
	// Also keep in mind that this isn't a limitation of the SIZE of the texutre image itself;
	//  you can have quite massive textures loaded (this is the primary reason GPU memories
	//  get bigger and bigger, sometimes larger than the CPU's RAM). 
	// So it's not like you have to have only one "image" per texture; often multiple textures 
	//  are combined into one, giant texture of multiple images. 
	//  (Google search for "texture atlas" to see what I mean - so you could have 
	//   at LEAST 16 of these suckers... that's a LOT of potential images)
	//
	//	+--------------+      +-----------------+     +---------------------------+
	//	|              |      |                 |     |                           |
	//	|              |      |                 |     |                           |
	//	|              |      |                 |     |                           |
	//	|              |      |                 |     |                           |
	//	| Texture Unit |    \ |  Binding point  | /   |      Shader sampler       |
	//	|              +----->+   TEXTURE UNIT  +<----+       (in shader)         |
	//	|              |    / |                 | \   | (uinform sampler2D, etc.) |
	//	|              |      |                 |     |                           |
	//	|              |      |                 |     |                           |
	//	|              |      |                 |     |                           |
	//	|              |      |                 |     |                           |
	//	|              |      |                 |     |                           |
	//	+--------------+      +-----------------+     +---------------------------+
	//
	// Last note: One of the biggest "gotchas" is that if you only have one texture, 
	// and one texture sampler, it all defaults to zero (0). This is fine, IF you
	// only have one texture, but the minute you have MORE than one texture 
	// (and to do anything interesting, you will need more than one), you have to 
	// deal with this "indirect binding" situation. 
	

	bool bNoErrors = true;

	// Step 1: Find the "texture number" for the image:
	//GLuint textureNum_Brick = 0;
	//if ( ! ::g_pTheTextureManager->GetTextureNumberFromName("brick_texture3325_square_powOf2.bmp", textureNum_Brick) )
	//{
	//	bNoErrors = false;
	//}

	//// Step 2: Assign this texture to a "binding point" in the GPU
	//// Pre OpenGL 4.5, these are given defines from GL_TEXTURE0 to GL_TEXTURE31
	//// We'll assign it to GL_TEXTURE0, just because. 
	//// 33,984
	//glActiveTexture( GL_TEXTURE0 );		// make this texture unit "active"
	//// From this point on, anything "texture stuff" we do is assigned to the "active" texture
	//// (which is a little indirect, but that's the way it is...)
	//ExitOnGLError("ERROR: AssignTextureUnitSimple()");

	//// Step 3: Assign this texture (the "active" one) to a "binding point".
	//// Note: we also have to be careful that the TYPE of texture is consistent. 
	////  In this case, the texture is a 2D texture, so we bind it as a 2D texture. 
	////  (If you don't do this, at best nothing will happen, at worst, your driver will crash)
	//// Note: these are also assigned the defines, so GL_TEXTURE0 in this case
	//glBindTexture( GL_TEXTURE_2D, textureNum_Brick );
	//ExitOnGLError("ERROR: AssignTextureUnitSimple()");

	//// Step 4: Now we tie the "binding point" to the shader "sampler"
	//// Here's where it gets annoying. While the texture units and binding points 
	////  are assigned pre-defined values, the samplers are assigned values from 0 and up. 
	//// So: GL_TEXTURE0 is given the value 0
	////     GL_TEXTURE1 is given the value 1
	////    ...and so on.
	//// In fact, you can actually use values of GL_TEXTURE0 + whateverOffsetYouWant
	////  in the previous calls - that's why, even though the maximum define is GL_TEXTURE31, 
	////  you can still assign 80 texture uints, by doing (GL_TEXTURE0 + 80), as long as 
	////  your GPU can handle that, of course. 
	//// 
	//// SERIOUS GOTCHA!!! Note we are passing zero (0) NOT GL_TEXTURE0 here....
	//glUniform1i( UniLoc_texSampler2D[0], 0 );   // --NOT-- GL_TEXTURE0!!!!
	//ExitOnGLError("ERROR: AssignTextureUnitSimple()");


	//// Now we'll do it for the the other textures...

	//{
	//	// Place the "blue tile" in binding point 1, and sampler 1
	//	GLuint textureNum_BlueTile = 0;
	//	if ( ! ::g_pTheTextureManager->GetTextureNumberFromName("Free_Texture_Digital_08.preview_square_powOf2.bmp", textureNum_BlueTile) )
	//	{
	//		bNoErrors = false;
	//	}
	//	glActiveTexture( GL_TEXTURE1 );		// make this texture unit "active"
	//	glBindTexture( GL_TEXTURE_2D, textureNum_BlueTile );
	//	glUniform1i( UniLoc_texSampler2D[1], 1 );		// Note the "1" instead of GL_TEXTURE1
	//}

	//{
	//	// Place the "Gold" in binding point 2, and sampler 2
	//	GLuint textureNum_Gold = 0;
	//	if ( ! ::g_pTheTextureManager->GetTextureNumberFromName("ttt-03_square_powOf2.bmp", textureNum_Gold) )
	//	{
	//		bNoErrors = false;
	//	}
	//	glActiveTexture( GL_TEXTURE2 );		// make this texture unit "active"
	//	glBindTexture( GL_TEXTURE_2D, textureNum_Gold );
	//	glUniform1i( UniLoc_texSampler2D[2], 2 );		// Note the "2" instead of GL_TEXTURE2
	//}

	//{
	//	// Place the "Blue whale" in binding point 3, and sampler 3
	//	GLuint textureNum_BlueWhale = 0;
	//	if ( ! ::g_pTheTextureManager->GetTextureNumberFromName("BlueWhale.bmp", textureNum_BlueWhale) )
	//	{
	//		bNoErrors = false;
	//	}
	//	glActiveTexture( GL_TEXTURE3 );		// make this texture unit "active"
	//	glBindTexture( GL_TEXTURE_2D, textureNum_BlueWhale );
	//	glUniform1i( UniLoc_texSampler2D[3], 3 );		// Note the "3" instead of GL_TEXTURE3
	//}

	// Or something like this...
	SetTextureBinding( "glass.bmp", GL_TEXTURE0, 0 );
	SetTextureBinding( "Free_Texture_Digital_08.preview_square_powOf2.bmp", GL_TEXTURE1, 1 );
	SetTextureBinding( "rock.bmp", GL_TEXTURE2, 2 );
	SetTextureBinding( "BlueWhale.bmp", GL_TEXTURE3, 3 );
	SetTextureBinding( "sand.bmp", GL_TEXTURE4, 4 );
	SetTextureBinding( "TropicalFish01.bmp", GL_TEXTURE5, 5 );
	SetTextureBinding( "TropicalFish03.bmp", GL_TEXTURE6, 6 );
	SetTextureBinding( "TropicalFish05.bmp", GL_TEXTURE7, 7 );			// <-- "Mask" texture
	SetTextureBinding( "TropicalFish02.bmp", GL_TEXTURE8, 8 );	// "explosion" texture
	SetTextureBinding( "TropicalFish04.bmp", GL_TEXTURE9, 9 );
	SetTextureBinding( "explode.bmp", GL_TEXTURE10, 10 );
	SetTextureBinding( "Fence_Mask.bmp", GL_TEXTURE11, 11 );
	return bNoErrors;
}

void RenderFunction(void)
{


	++FrameCount;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up a camera... 

	matView = glm::mat4(1.0f);

	// Look at the bunny
	//::g_cam_at = ::g_vec_pGOs[0]->position;

	//matView = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f),  // "eye"
		//                   glm::vec3(0.0f, 0.0f, 0.0f),   // "At"
		//                   glm::vec3(0.0f, 1.0f, 0.0f));  // up
	matView = glm::lookAt( ::g_cam_eye,  // "eye"
							::g_cam_at,   // "At"
						glm::vec3(0.0f, 1.0f, 0.0f));  // up

	::g_pTheShaderManager->UseShaderProgram("basicShader");

	glUniform3f( UniLoc_eye, ::g_cam_eye.x, ::g_cam_eye.y, ::g_cam_eye.z );

	SetLightUniforms();

	AssignTextureUnitsSimple();
	ExitOnGLError("ERROR: Could not set the shader uniforms");


	//glUniform4f( UniLoc_Light_0_position, 0.0f, 3.0f, 0.0f, 1.0f );	// "theLights[0].position"); 
	//glUniform4f( UniLoc_Light_0_ambient, 0.2f, 0.0f, 0.02f, 1.0f );	// "theLights[0].ambient");
	//glUniform4f( UniLoc_Light_0_diffuse, 1.0f, 0.0f, 0.0f, 1.0f );	// "theLights[0].diffuse");  
	//glUniform4f( UniLoc_Light_0_specular, 1.0f, 1.0f, 1.0f, 1.0f );	// "theLights[0].specular");  
	//glUniform1f( UniLoc_Light_0_attenConst, 0.1f );	// "theLights[0].attenConst");  
	//glUniform1f( UniLoc_Light_0_attenLinear, 0.1f );	// "theLights[0].attenLinear");  
	//glUniform1f( UniLoc_Light_0_attenQuad, 0.1f );	// "theLights[0].attenQuad");  
	//
	//glUniform4f( UniLoc_Light_1_position, 3.0f, 3.0f, 0.0f, 1.0f );	// "theLights[0].position"); 
	//glUniform4f( UniLoc_Light_1_ambient, 0.0f, 0.2f, 0.0f, 1.0f );	// "theLights[0].ambient");
	//glUniform4f( UniLoc_Light_1_diffuse, 0.0f, 1.0f, 0.0f, 1.0f );	// "theLights[0].diffuse");  
	//glUniform4f( UniLoc_Light_1_specular, 1.0f, 1.0f, 1.0f, 1.0f );	// "theLights[0].specular");  
	//glUniform1f( UniLoc_Light_1_attenConst, 0.1f );	// "theLights[0].attenConst");  
	//glUniform1f( UniLoc_Light_1_attenLinear, 0.1f );	// "theLights[0].attenLinear");  
	//glUniform1f( UniLoc_Light_1_attenQuad, 0.1f );	// "theLights[0].attenQuad");  

	// Put light where tie fighter is...
	::g_vecLights[1].position.x = ::g_vec_pGOs[0]->position.x - 1.0f;
	::g_vecLights[1].position.y = ::g_vec_pGOs[0]->position.y;
	::g_vecLights[1].position.z = ::g_vec_pGOs[0]->position.z;



	for (std::vector< cGameObject* >::iterator itGO = ::g_vec_pGOs.begin();
		itGO != ::g_vec_pGOs.end(); itGO++)
	{
		cGameObject* pCurGO = *itGO;
		DrawObject(pCurGO);
	}

	if ( g_bDebugLights )
	{
		// White, tiny ball at position
		::g_pDebugBall->position = glm::vec3(::g_vecLights[g_selectedLightIndex].position.x,
												::g_vecLights[g_selectedLightIndex].position.y, 
												::g_vecLights[g_selectedLightIndex].position.z);
		::g_pDebugBall->debugColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		::g_pDebugBall->bUseDebugColour = true;
		::g_pDebugBall->scale = 0.1f;
		DrawObject(::g_pDebugBall);

		// At 75% brightness
		::g_pDebugBall->debugColour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		::g_pDebugBall->bUseDebugColour = true;
		::g_pDebugBall->scale 
			= ::g_vecLights[g_selectedLightIndex].calcDistanceAtBrightness(0.75f);
		DrawObject(::g_pDebugBall);

		// at 50% brightness
		::g_pDebugBall->debugColour = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		::g_pDebugBall->scale 
			= ::g_vecLights[g_selectedLightIndex].calcDistanceAtBrightness(0.50f);
		DrawObject(::g_pDebugBall);

		// at 25% 
		::g_pDebugBall->scale = 1.0f/ 0.01f;		// quad atten
		::g_pDebugBall->debugColour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		::g_pDebugBall->scale 
			= ::g_vecLights[g_selectedLightIndex].calcDistanceAtBrightness(0.25f);
		DrawObject(::g_pDebugBall);
	}
  
	glutSwapBuffers();
}

void HandleIO(void)
{
	// Super Meat Boy...
	::g_vec_pGOs[0]->velocity = glm::vec3(0.0f);

	bool bBunnyMoved = false;
	
	// Thruster bunny (Dead Space 2??)
	//::g_vec_pGOs[0]->accel = glm::vec3(0.0f);
	//::g_vec_pGOs[0]->velocity *= 0.99f;	

	if ( (GetAsyncKeyState('1') & 0x8000) != 0 )
	{	// Increase attenuation (making it darker)
		::g_vecLights[::g_selectedLightIndex].attenLinear *= 1.01f;
	}
	if ( (GetAsyncKeyState('2') & 0x8000) != 0 )
	{	// Decrease attenuation (making it darker)
		::g_vecLights[::g_selectedLightIndex].attenLinear *= 0.99f;
	}
	if ( (GetAsyncKeyState('3') & 0x8000) != 0 )
	{	// Increase attenuation (making it darker)
		::g_vecLights[::g_selectedLightIndex].attenQuad *= 1.001f;
	}
	if ( (GetAsyncKeyState('4') & 0x8000) != 0 )
	{	// Decrease attenuation (making it darker)
		::g_vecLights[::g_selectedLightIndex].attenQuad *= 0.999f;
	}

	// Ctrl pressed, move selected light, otherwise, move object zero(0)
	bool bCrtlPressed = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) || (GetAsyncKeyState(VK_RCONTROL) & 0x8000);

	const float lightMoveSpeed = 0.1f;

	if ( bCrtlPressed ) 
	{
		if ( (GetAsyncKeyState('A') & 0x8000) != 0 )
		{ 
			::g_vecLights[::g_selectedLightIndex].position.x -= lightMoveSpeed;
		}
		if ( (GetAsyncKeyState('D') & 0x8000) != 0 )
		{ 
			::g_vecLights[::g_selectedLightIndex].position.x += lightMoveSpeed;
		}
		if ( (GetAsyncKeyState('W') & 0x8000) != 0 ) 
		{ 
			::g_vecLights[::g_selectedLightIndex].position.z += lightMoveSpeed;
		}
		if ( (GetAsyncKeyState('S') & 0x8000) != 0 ) 
		{ 
			::g_vecLights[::g_selectedLightIndex].position.z -= lightMoveSpeed;
		}
		if ( (GetAsyncKeyState('Q') & 0x8000) != 0 ) 
		{ 
			::g_vecLights[::g_selectedLightIndex].position.y += lightMoveSpeed;
		}
		if ( (GetAsyncKeyState('E') & 0x8000) != 0 ) 
		{ 
			::g_vecLights[::g_selectedLightIndex].position.y -= lightMoveSpeed;
		}
	}
	else
	{	// Move object zero
		// TODO: Sexy code...
		if ( (GetAsyncKeyState('A') & 0x8000) != 0 )
		{	// Move Bunny Left
			::g_vec_pGOs[0]->velocity.x = -2.0f;	bBunnyMoved = true;
		}
		if ( (GetAsyncKeyState('D') & 0x8000) != 0 )
		{	// Move Bunny Right
			::g_vec_pGOs[0]->velocity.x = +2.0f;	bBunnyMoved = true;
		}
		if ( (GetAsyncKeyState('W') & 0x8000) != 0 )
		{
			::g_vec_pGOs[0]->velocity.z = +2.0f;	bBunnyMoved = true;
		}
		if ( (GetAsyncKeyState('S') & 0x8000) != 0 )
		{
			::g_vec_pGOs[0]->velocity.z = -2.0f;	bBunnyMoved = true;
		}
		if ( (GetAsyncKeyState('Q') & 0x8000) != 0 )
		{
			::g_vec_pGOs[0]->velocity.y = +2.0f;	bBunnyMoved = true;
		}
		if ( (GetAsyncKeyState('E') & 0x8000) != 0 )
		{
			::g_vec_pGOs[0]->velocity.y = -2.0f;	bBunnyMoved = true;
		}
		if ( (GetAsyncKeyState('Z') & 0x8000) != 0 )
		{
			::g_vec_pGOs[0]->preRotation.z += 0.01f;
		}
		if ( (GetAsyncKeyState('C') & 0x8000) != 0 )
		{
			::g_vec_pGOs[0]->preRotation.z -= 0.01f;
		}

		if ( bBunnyMoved )
		{
			std::cout << ::g_vec_pGOs[0]->position.x << ", " 
				<< ::g_vec_pGOs[0]->position.y << ", " 
				<< ::g_vec_pGOs[0]->position.z << std::endl;
		}
	}  // if ( bCrtlPressed ) 


	return;
}

void IdleFunction(void)
{
	//// Look at the bunny
	//::g_cam_at = ::g_vec_pGOs[0]->position;
	//SHORT keyState_A = GetAsyncKeyState( 'A' );	
//
	//if ( ( keyState_A & 0x8000) != 0 )
	//{	// 'A' Key is down, yo
	//	std::cout << "A is down" << std::endl;
	//}
	//else
	//{
	//	std::cout << "A is up" << std::endl;
	//}

	HandleIO();

	// Get elapsed secs since last idle (likely 0.1 ms)
	float deltaTime = g_AniTimer.GetElapsedSeconds( true );

	// This is technically "Explicit forward Euler integration"
	cGameObject* pCurGO = 0;
	for ( std::vector< cGameObject* >::iterator itGO = ::g_vec_pGOs.begin();
		  itGO != ::g_vec_pGOs.end(); itGO++ )
	{
		pCurGO = *itGO;

		// Doing x, y, and z at the same time
		pCurGO->velocity += pCurGO->accel * deltaTime;		// 1.0;

		pCurGO->position += pCurGO->velocity * deltaTime; 

		//pCurGO->velocity.x += pCurGO->accel.x * deltaTime;
		//pCurGO->velocity.y += pCurGO->accel.y * deltaTime;
		//pCurGO->velocity.z += pCurGO->accel.z * deltaTime;
	}

	glutPostRedisplay();
}

void TimerFunction(int Value)
{
  //if (0 != Value) {
  //  char* TempString = (char*)
  //    malloc(512 + strlen(WINDOW_TITLE_PREFIX));
//
  //  sprintf(
  //    TempString,
  //    "%s: %d Frames Per Second @ %d x %d",
  //    WINDOW_TITLE_PREFIX,
  //    FrameCount * 4,
  //    CurrentWidth,
  //    CurrentHeight
  //  );

	std::stringstream ssTitle;
	ssTitle << std::fixed << std::setprecision(3) 
		<< "Light# (" << ::g_selectedLightIndex << "): "
		<< ::g_vecLights[::g_selectedLightIndex].position.x 
		<< ", " << ::g_vecLights[::g_selectedLightIndex].position.y
		<< ", " << ::g_vecLights[::g_selectedLightIndex].position.z
		<< "; "
		<< ::g_vecLights[::g_selectedLightIndex].attenLinear 
		<< "; "
		<< ::g_vecLights[::g_selectedLightIndex].attenQuad;

    glutSetWindowTitle(ssTitle.str().c_str());

    //glutSetWindowTitle(TempString);
    //free(TempString);

  //FrameCount = 0;
  glutTimerFunc(100, TimerFunction, 1);
}



bool bCheckShaderCompileStatus(GLuint shaderID, std::string errors)
{
	GLint statusOK; 
	::glGetObjectParameterivARB( shaderID , GL_OBJECT_COMPILE_STATUS_ARB, &statusOK );
	if ( !statusOK )
	{
		// This gets the 'last' error message for that shader (if there was one)
		GLcharARB infoLog[ GL_INFO_LOG_LENGTH ];	// defined in glext.h
		glGetInfoLogARB( shaderID , GL_INFO_LOG_LENGTH, NULL, infoLog );
		std::stringstream ss;
		ss << infoLog << std::endl;
		errors = ss.str();
		return false;
	}
	// No errors
	return true;
}

bool bCheckShaderLinkStatus(GLuint shaderID, std::string errors)
{
	GLint statusOK; 
	::glGetObjectParameterivARB( shaderID , GL_LINK_STATUS, &statusOK );
	if ( !statusOK )
	{
		// This gets the 'last' error message for that shader (if there was one)
		GLcharARB infoLog[ GL_INFO_LOG_LENGTH ];	// defined in glext.h
		glGetInfoLogARB( shaderID , GL_INFO_LOG_LENGTH, NULL, infoLog );
		std::stringstream ss;
		ss << infoLog << std::endl;
		errors = ss.str();
		return false;
	}
	// No errors
	return true;
}

GLint getLightUniformLocation( int shaderID, int index, std::string attribute )
{
	std::stringstream ssUniformName;
	ssUniformName << "theLights[" << index << "]." << attribute;
	return glGetUniformLocation(shaderID, ssUniformName.str().c_str() );
}

void SetUpUniformVariables(void)
{
	GLuint shaderID = ::g_pTheShaderManager->GetShaderIDFromName("basicShader");

	ModelMatrixUniformLocation
		= glGetUniformLocation(shaderID /*ShaderIds[0]*/, "ModelMatrix");

	ViewMatrixUniformLocation
		= glGetUniformLocation(shaderID /*ShaderIds[0]*/, "ViewMatrix");

	ProjectionMatrixUniformLocation
		= glGetUniformLocation(shaderID /*ShaderIds[0]*/, "ProjectionMatrix");

	ModelMatrixRotationOnlyUniformLocation
		= glGetUniformLocation(shaderID, "ModelMatrixRotOnly");


	UniLoc_MaterialAmbient_RGB = glGetUniformLocation(shaderID, "myMaterialAmbient_RGB");
	UniLoc_MaterialDiffuse_RGB = glGetUniformLocation(shaderID, "myMaterialDiffuse_RGB");
	UniLoc_MaterialSpecular = glGetUniformLocation(shaderID, "myMaterialSpecular"); 
	UniLoc_MaterialShininess = glGetUniformLocation(shaderID, "myMaterialShininess"); 

	UniLoc_eye = glGetUniformLocation(shaderID, "eye"); 

	UniLoc_debugColour = glGetUniformLocation(shaderID,  "debugColour");
	UniLoc_bUseDebugColour = glGetUniformLocation(shaderID,  "bUseDebugColour");
	UniLoc_bUseVertexRGBAColours = glGetUniformLocation(shaderID,  "bUseVertexRGBAColours");
	UniLoc_bUseTextureMaterials = glGetUniformLocation(shaderID,  "bUseTextureMaterials");
	UniLoc_bUseTexturesOnly = glGetUniformLocation(shaderID, "bUseTexturesNoLighting");

	UniLoc_myAlphaAllObject = glGetUniformLocation(shaderID, "myAlphaAllObject" );
	UniLoc_bAlphaForEntireObject = glGetUniformLocation(shaderID, "bAlphaForEntireObject" );

	UniLoc_bUseDiscardMask =  glGetUniformLocation(shaderID, "bUseDiscardMask" );

	for ( int index = 0; index != NUMBEROFLIGHTS; index++ )
	{
		cLightDesc curLight;
		
		//{
		//	std::stringstream ssPosition;
		//	ssPosition << "theLights[" << index << "].position";
		//	//UniLoc_Light_0_position = glGetUniformLocation(shaderID, "theLights[0].position"); 
		//	curLight.UniLoc_position = glGetUniformLocation(shaderID, ssPosition.str().c_str() );
		//}
		//{
		//	std::stringstream ssAmbient;		
		//	ssAmbient << "theLights[" << index << "].ambient";
		//	curLight.UniLoc_ambient = glGetUniformLocation(shaderID, ssAmbient.str().c_str() );
		//}
		//{
		//	std::stringstream ssDiffuse;		
		//	ssDiffuse << "theLights[" << index << "].diffuse";
		//	curLight.UniLoc_diffuse = glGetUniformLocation(shaderID, ssDiffuse.str().c_str() );
		//}
		//{
		//	std::stringstream ssSpecular;		
		//	ssSpecular << "theLights[" << index << "].specular";
		//	curLight.UniLoc_specular = glGetUniformLocation(shaderID, ssSpecular.str().c_str() );
		//}
		//{
		//	std::stringstream ssAttenConst;		
		//	ssAttenConst << "theLights[" << index << "].attenConst";
		//	curLight.UniLoc_attenConst = glGetUniformLocation(shaderID, ssAttenConst.str().c_str() );
		//}
		//{
		//	std::stringstream ssAttenLinear;		
		//	ssAttenLinear << "theLights[" << index << "].attenLinear";
		//	curLight.UniLoc_attenLinear = glGetUniformLocation(shaderID, ssAttenLinear.str().c_str() );
		//}
		//{
		//	std::stringstream ssAttenQuad;		
		//	ssAttenQuad << "theLights[" << index << "].attenQuad";
		//	curLight.UniLoc_attenQuad = glGetUniformLocation(shaderID, ssAttenQuad.str().c_str() );
		//}

		curLight.UniLoc_position = getLightUniformLocation( shaderID, index, "position" );
		curLight.UniLoc_ambient = getLightUniformLocation( shaderID, index, "ambient" );
		curLight.UniLoc_diffuse = getLightUniformLocation( shaderID, index, "diffuse" );
		curLight.UniLoc_specular = getLightUniformLocation( shaderID, index, "specular" );
		curLight.UniLoc_attenConst = getLightUniformLocation( shaderID, index, "attenConst" );
		curLight.UniLoc_attenLinear = getLightUniformLocation( shaderID, index, "attenLinear" );
		curLight.UniLoc_attenQuad = getLightUniformLocation( shaderID, index, "attenQuad" );

		curLight.UniLoc_type = getLightUniformLocation( shaderID, index, "lightType" );

		// For the spot and directional lights
		curLight.UniLoc_direction = getLightUniformLocation( shaderID, index, "direction" );
		// For the spot lights
		curLight.UniLoc_anglePenumbraStart = getLightUniformLocation( shaderID, index, "anglePenumbraStart" );
		curLight.UniLoc_anglePenumbraEnd = getLightUniformLocation( shaderID, index, "anglePenumbraEnd" );
		
		::g_vecLights.push_back( curLight );
	}


 /*

	UniLoc_Light_0_attenQuad = glGetUniformLocation(shaderID, "theLights[0].attenQuad");  

	UniLoc_Light_1_position = glGetUniformLocation(shaderID, "theLights[1].position"); 
	UniLoc_Light_1_ambient = glGetUniformLocation(shaderID, "theLights[1].ambient");
	UniLoc_Light_1_diffuse = glGetUniformLocation(shaderID, "theLights[1].diffuse");  
	UniLoc_Light_1_specular = glGetUniformLocation(shaderID, "theLights[1].specular");  
	UniLoc_Light_1_attenConst = glGetUniformLocation(shaderID, "theLights[1].attenConst");  
	UniLoc_Light_1_attenLinear = glGetUniformLocation(shaderID, "theLights[1].attenLinear");  
	UniLoc_Light_1_attenQuad = glGetUniformLocation(shaderID, "theLights[1].attenQuad");  */


	ExitOnGLError("ERROR in SetUpUniformVariables(): Could not get shader uniform locations");

	return;
}

bool SetUpTextures(void)
{
	::g_pTheTextureManager = new CTextureManager();

	bool bItsAllGoodMan = true;

	::g_pTheTextureManager->setBasePath("assets/textures");
	
	if ( ! ::g_pTheTextureManager->Create2DTextureFromBMPFile("glass.bmp", true) )
	{
		std::cout << "Couldn't load texture." << std::endl;
		bItsAllGoodMan = false;
	}

	if ( ! ::g_pTheTextureManager->Create2DTextureFromBMPFile("Free_Texture_Digital_08.preview_square_powOf2.bmp", true) )
	{
		std::cout << "Couldn't load texture." << std::endl;
		bItsAllGoodMan = false;
	}


	if ( ! ::g_pTheTextureManager->Create2DTextureFromBMPFile("BlueWhale.bmp", true) )
	{
		std::cout << "Couldn't load texture." << std::endl;
		bItsAllGoodMan = false;
	}

	if ( ! ::g_pTheTextureManager->Create2DTextureFromBMPFile("sand.bmp", true) )
	{
		std::cout << "Couldn't load texture." << std::endl;
		bItsAllGoodMan = false;
	}

	if ( ! ::g_pTheTextureManager->Create2DTextureFromBMPFile("TropicalFish01.bmp", true) )
	{
		std::cout << "Couldn't load texture." << std::endl;
		bItsAllGoodMan = false;
	}
	if ( ! ::g_pTheTextureManager->Create2DTextureFromBMPFile("TropicalFish02.bmp", true) )
	{
		std::cout << "Couldn't load texture." << std::endl;
		bItsAllGoodMan = false;
	}
	// For the "fireball" thingy...
	if ( ! ::g_pTheTextureManager->Create2DTextureFromBMPFile("TropicalFish03.bmp", true) )	// <--- mask image
	{
		std::cout << "Couldn't load texture." << std::endl;
		bItsAllGoodMan = false;
	}
	if ( ! ::g_pTheTextureManager->Create2DTextureFromBMPFile("TropicalFish05.bmp", true) )	// <--- "explosion" texture
	{
		std::cout << "Couldn't load texture." << std::endl;
		bItsAllGoodMan = false;
	}
	if (!::g_pTheTextureManager->Create2DTextureFromBMPFile("TropicalFish04.bmp", true))	// <--- "explosion" texture
	{
		std::cout << "Couldn't load texture." << std::endl;
		bItsAllGoodMan = false;
	}
	if (!::g_pTheTextureManager->Create2DTextureFromBMPFile("explode.bmp", true))	// <--- "explosion" texture
	{
		std::cout << "Couldn't load texture." << std::endl;
		bItsAllGoodMan = false;
	}
	if (!::g_pTheTextureManager->Create2DTextureFromBMPFile("ttt-03_square_powOf2.bmp", true))	// <--- "explosion" texture
	{
		std::cout << "Couldn't load texture." << std::endl;
		bItsAllGoodMan = false;
	}
	if (!::g_pTheTextureManager->Create2DTextureFromBMPFile("Fence_Mask.bmp", true))	// <--- "explosion" texture
	{
		std::cout << "Couldn't load texture." << std::endl;
		bItsAllGoodMan = false;
	}
	// Now we set up the sampler uniform locations. 
	// These are exactly the same as any other uniforms we've used, as they 
	//  represent a register in the GPU. Note that you CAN'T have sampler 
	//  arrays like you can with the lights. You CAN have sampler arrays, but they 
	//  are a very different thing: an "array" of 2D texture samplers is actually 
	//  implemented as a single 3D texture sampler, where you index the values by 
	//  the z (u) coordinate. We AREN'T looking at those as it's someone advanced. 
	// Bottom line: you have to get these one by one... 

	GLuint shaderID = ::g_pTheShaderManager->GetShaderIDFromName("basicShader");

	UniLoc_texSampler2D[0] = glGetUniformLocation(shaderID, "texSamp2D_00" );
	UniLoc_texSampler2D[1] = glGetUniformLocation(shaderID, "texSamp2D_01" );
	UniLoc_texSampler2D[2] = glGetUniformLocation(shaderID, "texSamp2D_02" );
	UniLoc_texSampler2D[3] = glGetUniformLocation(shaderID, "texSamp2D_03" );
	UniLoc_texSampler2D[4] = glGetUniformLocation(shaderID, "texSamp2D_04" );
	UniLoc_texSampler2D[5] = glGetUniformLocation(shaderID, "texSamp2D_05" );
	UniLoc_texSampler2D[6] = glGetUniformLocation(shaderID, "texSamp2D_06" );
	UniLoc_texSampler2D[7] = glGetUniformLocation(shaderID, "texSamp2D_07" );
	UniLoc_texSampler2D[8] = glGetUniformLocation(shaderID, "texSamp2D_08" ); //return -1 Y??
	UniLoc_texSampler2D[9] = glGetUniformLocation(shaderID, "texSamp2D_09" );
	UniLoc_texSampler2D[10] = glGetUniformLocation(shaderID, "texSamp2D_10" );
	UniLoc_texSampler2D[11] = glGetUniformLocation(shaderID, "texSamp2D_11" );

	UniLoc_texMix[0] = glGetUniformLocation(shaderID, "textureMixRatios[0]");
	UniLoc_texMix[1] = glGetUniformLocation(shaderID, "textureMixRatios[1]");
	UniLoc_texMix[2] = glGetUniformLocation(shaderID, "textureMixRatios[2]");
	UniLoc_texMix[3] = glGetUniformLocation(shaderID, "textureMixRatios[3]");
	UniLoc_texMix[4] = glGetUniformLocation(shaderID, "textureMixRatios[4]");
	UniLoc_texMix[5] = glGetUniformLocation(shaderID, "textureMixRatios[5]");
	UniLoc_texMix[6] = glGetUniformLocation(shaderID, "textureMixRatios[6]");
	UniLoc_texMix[7] = glGetUniformLocation(shaderID, "textureMixRatios[7]");
	UniLoc_texMix[8] = glGetUniformLocation(shaderID, "textureMixRatios[8]");
	UniLoc_texMix[9] = glGetUniformLocation(shaderID, "textureMixRatios[9]");
	UniLoc_texMix[10] = glGetUniformLocation(shaderID, "textureMixRatios[10]");
	UniLoc_texMix[11] = glGetUniformLocation(shaderID, "textureMixRatios[11]");

	ExitOnGLError("ERROR in SetUpTextures().");


	return bItsAllGoodMan;
}

void SetupShader(void)
{
//	std::string error;
//	
	::g_pTheShaderManager = new CGLShaderManager();
	
	CShaderDescription vertShader;
//	vertShader.filename = "assets/shaders/SimpleShader.vertex.glsl";
	vertShader.filename = "assets/shaders/MultiLightsTextures.vertex.glsl";
	vertShader.name = "basicVert";
	vertShader.type = GLSHADERTYPES::VERTEX_SHADER;
	
	CShaderDescription fragShader;
//	fragShader.filename = "assets/shaders/SimpleShader.fragment.glsl";
//	fragShader.filename = "assets/shaders/MultiLightsTextures.fragment.glsl";
	fragShader.filename = "assets/shaders/MultiLightsTextures.fragment.spot.glsl";
	fragShader.name = "basicFrag";
	fragShader.type = GLSHADERTYPES::FRAGMENT_SHADER;
	
	CShaderProgramDescription basicShaderProg;
	basicShaderProg.name = "basicShader";
	basicShaderProg.vShader = vertShader;
	basicShaderProg.fShader = fragShader;
	
	if ( ! ::g_pTheShaderManager->CreateShaderProgramFromFile(basicShaderProg) )
	{	// Oh no, Mr. Bill!
		if ( ! basicShaderProg.bIsOK )
		{
			std::cout << "Error compiling the shader program" << std::endl;
			// Insert sexy error handling, logging code here... 
			for (std::vector<std::string>::iterator itError 
				 = basicShaderProg.vecErrors.begin();
				 itError != basicShaderProg.vecErrors.end(); 
				 itError++ )
			{
				std::cout << *itError << std::endl;
			}
			for (std::vector<std::string>::iterator itError 
				 = basicShaderProg.vShader.vecShaderErrors.begin();
				 itError != basicShaderProg.vShader.vecShaderErrors.end(); 
				 itError++ )
			{
				std::cout << *itError << std::endl;
			}	
			for (std::vector<std::string>::iterator itError 
				 = basicShaderProg.fShader.vecShaderErrors.begin();
				 itError != basicShaderProg.fShader.vecShaderErrors.end(); 
				 itError++ )
			{
				std::cout << *itError << std::endl;
			}		
		}
	}// if ( ! ::g_pTheShaderManager->...
	
	// Assume we are good to go...
	::g_pTheShaderManager->UseShaderProgram("basicShader");

	{
		CShaderProgramDescription compiledShader;
		compiledShader.name = "basicShader";
		::g_pTheShaderManager->GetShaderProgramInfo(compiledShader);
		for ( std::vector< CShaderUniformDescription>::iterator itUniform = compiledShader.vecUniformVariables.begin(); 
			  itUniform != compiledShader.vecUniformVariables.end(); itUniform++ )
		{
			std::cout << itUniform->index << ":" <<  itUniform->name << std::endl;
		}
	}

	//ShaderIds[0] = glCreateProgram();
	//ExitOnGLError("ERROR: Could not create the shader program");
	//{
	//	ShaderIds[1] = LoadShader("assets/shaders/SimpleShader.fragment.glsl", GL_FRAGMENT_SHADER);
	//	ShaderIds[2] = LoadShader("assets/shaders/SimpleShader.vertex.glsl", GL_VERTEX_SHADER);
	//	glAttachShader(ShaderIds[0], ShaderIds[1]);
	//	glAttachShader(ShaderIds[0], ShaderIds[2]);
	//}
	//glLinkProgram(ShaderIds[0]);
	//ExitOnGLError("ERROR: Could not link the shader program");


	SetUpUniformVariables();

	if ( ! SetUpTextures() )
	{
		std::cout << "Warning: One or more textures didn't load." << std::endl;
	}

	ExitOnGLError("ERROR in SetUpShaders()");

	return;
}

// Was "void DestroyCube()"
void ShutErDownPeople(void)
{
	//glDetachShader(ShaderIds[0], ShaderIds[1]);
	//glDetachShader(ShaderIds[0], ShaderIds[2]);
	//glDeleteShader(ShaderIds[1]);
	//glDeleteShader(ShaderIds[2]);
	//glDeleteProgram(ShaderIds[0]);
	//ExitOnGLError("ERROR: Could not destroy the shaders");

	::g_pTheShaderManager->ShutDown();

	::g_pTheMeshManager->ShutDown();

	::g_pTheTextureManager->ShutDown();

	// Go through the game object vector, deleting everything
	for ( std::vector< cGameObject* >::iterator itpGO = ::g_vec_pGOs.begin();
		itpGO != ::g_vec_pGOs.end(); itpGO++ )
	{
		// A little verbose, but maybe more clear?
		cGameObject* pCurGO = *itpGO;
		delete pCurGO;
	}
	::g_vec_pGOs.clear();


	//  glDeleteBuffers(2, &BufferIds[1]);
	//  glDeleteVertexArrays(1, &BufferIds[0]);

	delete ::g_pTheMeshManager;		
	delete ::g_pTheShaderManager;
	delete ::g_pTheTextureManager;

	ExitOnGLError("ERROR: Could not destroy the buffer objects");
}

//void DrawCube(void)
void DrawObject( cGameObject* pGO )
{
	if ( ! pGO->bIsVisible )
	{
		return;
	}

  float CubeAngle;
  clock_t Now = clock();

  if (LastTime == 0)
    LastTime = Now;

  CubeRotation += 45.0f * ((float)(Now - LastTime) / CLOCKS_PER_SEC);
  CubeAngle = DegreesToRadians(CubeRotation);
  LastTime = Now;

//  ModelMatrix = IDENTITY_MATRIX;
//  RotateAboutY(&ModelMatrix, CubeAngle);
//  RotateAboutX(&ModelMatrix, CubeAngle);
  //matView = glm::mat4(1.0f);

  matWorld = glm::mat4(1.0f);		// identity matrix

  glm::mat4 matWorldRotOnly(1.0f);		// For later (lighting)

  // STARTOF: From the Guts file... 
  // Rotation (post)
  matWorld = glm::rotate(matWorld, pGO->postRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
  matWorld = glm::rotate(matWorld, pGO->postRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
  matWorld = glm::rotate(matWorld, pGO->postRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

  //matWorldRotOnly = glm::rotate(matWorldRotOnly, pGO->postRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
  //matWorldRotOnly = glm::rotate(matWorldRotOnly, pGO->postRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
  //matWorldRotOnly = glm::rotate(matWorldRotOnly, pGO->postRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

  // Translation 
  matWorld = glm::translate(matWorld, 
	                        glm::vec3(pGO->position.x,
								      pGO->position.y,
								      pGO->position.z));

  // Rotation (pre)
  matWorld = glm::rotate(matWorld, pGO->preRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
  matWorld = glm::rotate(matWorld, pGO->preRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
  matWorld = glm::rotate(matWorld, pGO->preRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

  //matWorldRotOnly = glm::rotate(matWorldRotOnly, pGO->preRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
  //matWorldRotOnly = glm::rotate(matWorldRotOnly, pGO->preRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
  //matWorldRotOnly = glm::rotate(matWorldRotOnly, pGO->preRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

  // Or you can use an inverse transpose of the model matrix.
  // Strips off translation and scaling
  matWorldRotOnly = glm::inverse(glm::transpose(matWorld));

  // Scale 
  matWorld = glm::scale(matWorld, glm::vec3(pGO->scale, 
	                                        pGO->scale,
	                                        pGO->scale));
  // ENDOF: From the Guts file

  //glUseProgram(ShaderIds[0]);
  //ExitOnGLError("ERROR: Could not use the shader program");

  ::g_pTheShaderManager->UseShaderProgram("basicShader");


  glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, 
	                 glm::value_ptr(matWorld) );

  glUniformMatrix4fv(ViewMatrixUniformLocation, 1, GL_FALSE, 
	                 glm::value_ptr(matView) );

  glUniformMatrix4fv(ModelMatrixRotationOnlyUniformLocation, 1, GL_FALSE, 
	                 glm::value_ptr(matWorldRotOnly) );

	if ( pGO->bUseDebugColour )
	{
		glUniform4f( UniLoc_debugColour, pGO->debugColour.r, pGO->debugColour.g, 
					                     pGO->debugColour.b, pGO->debugColour.a );
		// Boolean 
		glUniform1f( UniLoc_bUseDebugColour, 1.0f /*TRUE*/ );	// Non zero
		//glUniform1i( UniLoc_bUseDebugColour, 1 /*TRUE*/ );	// Non zero)
	}
	else
	{	// Set the unform material stuff for the object
		glUniform1f( UniLoc_bUseDebugColour, 0.0f /*FALSE*/ );	// Non zero

		glUniform3f( UniLoc_MaterialAmbient_RGB, pGO->ambient.r, pGO->ambient.g, pGO->ambient.b );
		glUniform3f( UniLoc_MaterialDiffuse_RGB, pGO->diffuse.r, pGO->diffuse.g, pGO->diffuse.b );
		glUniform3f( UniLoc_MaterialSpecular, pGO->specular.r, pGO->specular.g, pGO->specular.b );
		glUniform1f( UniLoc_MaterialShininess, pGO->shininess );
	}
 
	if ( pGO->bUseVertexRGBAColoursAsMaterials )
	{
		glUniform1f( UniLoc_bUseVertexRGBAColours, 1.0f /*TRUE*/ );	// Non zero = true
	}
	else
	{
		glUniform1f( UniLoc_bUseVertexRGBAColours, 0.0f /*FALSE*/ );	// Non zero = true
	}

	// ********************************************************************************************
	// Textures.... 
	if ( pGO->bUseTexturesAsMaterials ) 
	{
		glUniform1f( UniLoc_bUseTextureMaterials, 1.0f /*TRUE*/ );		// Non zero = true
	}
	else
	{
		glUniform1f( UniLoc_bUseTextureMaterials, 0.0f /*FALSE*/ );		// Non zero = true
	}//if ( pGO->bUseTexturesAsMaterials ) 

	if ( pGO->bUseTexturesWithNoLighting )
	{
		glUniform1f( UniLoc_bUseTexturesOnly, 1.0f /*TRUE*/ );
	}
	else
	{
		glUniform1f( UniLoc_bUseTexturesOnly, 0.0f /*FALSE*/ );
	}//if ( pGO->bUseTexturesWithNoLighting )

	// ********************************************************************************************
	// If we are using textures, set the "texture mix" values for this object
	if ( pGO->bUseTexturesAsMaterials || pGO->bUseTexturesWithNoLighting )
	{
		for ( unsigned int index = 0; index != NUMBEROF2DSAMPLERS; index++ )
		{	// Double-check that there actually IS a mix value in the object...
			if ( index < pGO->vecTextureMixRatios.size()  )
			{	// Set it from the object
				glUniform1f( ::UniLoc_texMix[index], pGO->vecTextureMixRatios[index] );
			}
			else
			{	// This object doesn't have a mix value at that location
				glUniform1f( ::UniLoc_texMix[index], 0.0f );
			}
		}
	}//if ( pGO->bUseTexturesAsMaterials || pGO->bUseTexturesWithNoLighting )


	ExitOnGLError("ERROR: Could not set the shader uniforms");

	std::string modelToDraw = pGO->modelName;

	cVBOInfo curVBO;
	if (!::g_pTheMeshManager->LookUpVBOInfoFromModelName(modelToDraw, curVBO))
	{ // Didn't find it.
		return;
	}

//  glBindVertexArray(BufferIds[0]);
  glBindVertexArray(curVBO.VBO_ID);


  ExitOnGLError("ERROR: Could not bind the VAO for drawing purposes");

  // Make everything lines ("wireframe")
  if ( pGO->bIsWireframe )
  {
	  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	  glDisable(GL_CULL_FACE);	// Enable "backface culling
  }
  else
  {  
	  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	  glEnable(GL_CULL_FACE);	// Enable "backface culling
  }


//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//glDisable(GL_CULL_FACE);	// Enable "backface culling
	  
  // Transparency
	glEnable( GL_BLEND );		// Enables "blending"
	// Source == already on framebuffer
	// Dest == what you're about to draw
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if ( pGO->bIsEntirelyTransparent )
	{	// Set alpha for ENTIRE object
		glUniform1f( UniLoc_bAlphaForEntireObject, 1.0f /*TRUE*/ );
		glUniform1f( UniLoc_myAlphaAllObject, pGO->alphaValue);	// 1.0 is NOT transparent	
	}
	else
	{	// Don't GLOBALLY set transparency
		glUniform1f( UniLoc_bAlphaForEntireObject, 0.0f /*FALSE*/ );
		glUniform1f( UniLoc_myAlphaAllObject, 1.0f );	// 1.0 is NOT transparent	
	}

	if ( pGO->bUseDiscardMask )
	{	// Set up discard mask in shader
		glUniform1f( UniLoc_bUseDiscardMask, 1.0f /*TRUE*/ );
	}
	else
	{	
		glUniform1f( UniLoc_bUseDiscardMask, 0.0f /*TRUE*/ );
	}



  unsigned int numberOfIndicesToDraw = curVBO.numberOfTriangles * 3;

  glDrawElements(GL_TRIANGLES, numberOfIndicesToDraw,	// 36,
	             GL_UNSIGNED_INT, 
	             (GLvoid*)0);
  ExitOnGLError("ERROR: Could not draw the cube");

  glBindVertexArray(0);
//  glUseProgram(0);
  ::g_pTheShaderManager->UseShaderProgram(0);

  return;
}


void SetUpInitialLightValues(void)
{
	// Assume the lights are loaded into the g_vecLights;
	::g_vecLights[0].position = glm::vec4( 4.0f, 3.0f, 0.0f, 1.0f );
	::g_vecLights[0].diffuse =glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	::g_vecLights[0].ambient =glm::vec4( 0.2f, 0.2f, 0.2f, 1.0f );
	::g_vecLights[0].specular =glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	::g_vecLights[0].attenConst = 0.0f;
	::g_vecLights[0].attenLinear = 0.2f;
	::g_vecLights[0].attenQuad = 0.1f;

	// Assume the lights are loaded into the g_vecLights;
	//::g_vecLights[0].position = glm::vec4( 5.0f, 9.6f, -0.7f, 1.0f );
	//::g_vecLights[0].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//::g_vecLights[0].ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	//::g_vecLights[0].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//::g_vecLights[0].attenConst = 0.0f;
	//::g_vecLights[0].attenLinear = 0.008f;
	//::g_vecLights[0].attenQuad = 0.05f;
	
	//::g_vecLights[0].lightType = 0;	// Spot
	::g_vecLights[0].lightType = 0;	// point
	//::g_vecLights[0].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	//::g_vecLights[0].anglePenumbraStart = 15.0f;
	//::g_vecLights[0].anglePenumbraEnd = 30.0f;
}


void CreateTheObjects(void)
{
	// Now with more ply...
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/BlueWhale.ply");
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/tankFrame.ply");
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/tankGlass.ply");
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/tankGround.ply");
	//plants
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/Plant1.ply");
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/Plant2.ply");
	//rocks
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/asteroid_sc0001.ply");
	//fishes
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/TropicalFish01.ply");
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/TropicalFish03.ply");
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/TropicalFish05.ply");
	//something else
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/castleTower.ply");

	//extra
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/TropicalFish02.ply");
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/TropicalFish04.ply");

	//g_pDebugBall = new cGameObject();
	//g_pDebugBall->modelName = "assets/models/Isoshphere.ply";
	//g_pDebugBall->debugColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//g_pDebugBall->bUseDebugColour = true;
	//g_pDebugBall->bIsWireframe = true;
	//g_pDebugBall->scale = 1.0f;
	//g_pDebugBall->bIsADebugObject = true;

	//cGameObject* pInvertBall = new cGameObject();
	//pInvertBall = new cGameObject();
	//pInvertBall->modelName = "assets/models/Isoshphere_normalsFacingIn.ply";
	////pInvertBall->debugColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	////pInvertBall->bUseDebugColour = true;
	//pInvertBall->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//pInvertBall->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	//pInvertBall->scale = 3.0f;
	//pInvertBall->position.z = 2.0f;
	//pInvertBall->position.y = 2.0f;
	cGameObject* pCastle = new cGameObject();
	pCastle->modelName = "assets/models/castleTower.ply";
	pCastle->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pCastle->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pCastle->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pCastle->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pCastle->preRotation.x = glm::radians(-90.0f);
	pCastle->scale = 0.03f;
	pCastle->position.z = -2.0f;
	pCastle->position.x = 1.5f;
	pCastle->position.y = -4.7f;
	pCastle->bUseTexturesAsMaterials = true;
	pCastle->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pCastle->vecTextureMixRatios[4] = 1.0f;		// fish1

	cGameObject* pFish1 = new cGameObject();
	pFish1->modelName = "assets/models/TropicalFish01.ply";
	pFish1->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pFish1->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pFish1->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pFish1->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pFish1->scale = 3.1f;
	pFish1->position.z = -2.0f;
	pFish1->position.x = 6.5f;
	pFish1->position.y = 0.0f;
	pFish1->bUseTexturesAsMaterials = true;
	pFish1->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pFish1->vecTextureMixRatios[5] = 1.0f;		// fish1

	cGameObject* pFish2 = new cGameObject();
	pFish2->modelName = "assets/models/TropicalFish01.ply";
	pFish2->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pFish2->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pFish2->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pFish2->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pFish2->scale = 3.1f;
	pFish2->position.z = -2.0f;
	pFish2->position.x = 1.5f;
	pFish2->position.y = 0.0f;
	pFish2->bUseTexturesAsMaterials = true;
	pFish2->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pFish2->vecTextureMixRatios[5] = 1.0f;		// fish1

	cGameObject* pFish3 = new cGameObject();
	pFish3->modelName = "assets/models/TropicalFish01.ply";
	pFish3->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pFish3->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pFish3->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pFish3->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pFish3->scale = 3.1f;
	pFish3->position.z = -2.0f;
	pFish3->position.x = -3.5f;
	pFish3->position.y = 0.0f;
	pFish3->bUseTexturesAsMaterials = true;
	pFish3->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pFish3->vecTextureMixRatios[5] = 1.0f;		// fish1

	cGameObject* pFish4 = new cGameObject();
	pFish4->modelName = "assets/models/TropicalFish03.ply";
	pFish4->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pFish4->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pFish4->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pFish4->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pFish4->scale = 3.1f;
	pFish4->position.z = -2.0f;
	pFish4->position.x = -4.5f;
	pFish4->position.y = 2.0f;
	pFish4->bUseTexturesAsMaterials = true;
	pFish4->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pFish4->vecTextureMixRatios[6] = 1.0f;		// fish1

	cGameObject* pFish5 = new cGameObject();
	pFish5->modelName = "assets/models/TropicalFish03.ply";
	pFish5->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pFish5->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pFish5->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pFish5->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pFish5->scale = 3.1f;
	pFish5->position.z = -2.0f;
	pFish5->position.x = -1.5f;
	pFish5->position.y = 2.0f;
	pFish5->bUseTexturesAsMaterials = true;
	pFish5->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pFish5->vecTextureMixRatios[6] = 1.0f;		// fish1

	cGameObject* pFish6 = new cGameObject();
	pFish6->modelName = "assets/models/TropicalFish05.ply";
	pFish6->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pFish6->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pFish6->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pFish6->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pFish6->scale = 3.1f;
	pFish6->position.z = -2.0f;
	pFish6->position.x = -1.5f;
	pFish6->position.y = 4.0f;
	pFish6->bUseTexturesAsMaterials = true;
	pFish6->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pFish6->vecTextureMixRatios[7] = 1.0f;		// fish1

	cGameObject* pFish7 = new cGameObject();
	pFish7->modelName = "assets/models/TropicalFish05.ply";
	pFish7->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pFish7->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pFish7->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pFish7->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pFish7->scale = 3.1f;
	pFish7->position.z = -2.0f;
	pFish7->position.x = 3.5f;
	pFish7->position.y = 4.0f;
	pFish7->bUseTexturesAsMaterials = true;
	pFish7->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pFish7->vecTextureMixRatios[7] = 1.0f;		// fish1

	cGameObject* pFish8 = new cGameObject();
	pFish8->modelName = "assets/models/TropicalFish02.ply";
	pFish8->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pFish8->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pFish8->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pFish8->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pFish8->scale = 3.1f;
	pFish8->position.z = -2.0f;
	pFish8->position.x = 7.5f;
	pFish8->position.y = 4.0f;
	pFish8->bUseTexturesAsMaterials = true;
	pFish8->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pFish8->vecTextureMixRatios[8] = 1.0f;		// fish1

	cGameObject* pFish9 = new cGameObject();
	pFish9->modelName = "assets/models/TropicalFish04.ply";
	pFish9->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pFish9->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pFish9->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pFish9->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pFish9->scale = 3.1f;
	pFish9->position.z = -2.0f;
	pFish9->position.x = 0.5f;
	pFish9->position.y = 5.0f;
	pFish9->bUseTexturesAsMaterials = true;
	pFish9->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pFish9->vecTextureMixRatios[9] = 1.0f;		// fish1

	cGameObject* pFish10 = new cGameObject();
	pFish10->modelName = "assets/models/TropicalFish04.ply";
	pFish10->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pFish10->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pFish10->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pFish10->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pFish10->scale = 3.1f;
	pFish10->position.z = -2.0f;
	pFish10->position.x = 4.5f;
	pFish10->position.y = 5.0f;
	pFish10->bUseTexturesAsMaterials = true;
	pFish10->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pFish10->vecTextureMixRatios[10] = 1.0f;		// fish1

	cGameObject* pFish11 = new cGameObject();
	pFish11->modelName = "assets/models/TropicalFish04.ply";
	pFish11->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pFish11->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pFish11->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pFish11->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pFish11->scale = 3.1f;
	pFish11->position.z = -2.0f;
	pFish11->position.x = 8.5f;
	pFish11->position.y = 0.0f;
	pFish11->bUseTexturesAsMaterials = true;
	pFish11->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pFish11->vecTextureMixRatios[11] = 1.0f;		// fish1

	cGameObject* pRock1 = new cGameObject();
	pRock1->modelName = "assets/models/asteroid_sc0001.ply";
	pRock1->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pRock1->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pRock1->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pRock1->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pRock1->scale = 0.2f;
	pRock1->position.z = -2.0f;
	pRock1->position.x = 6.5f;
	pRock1->position.y = -5.0f;
	pRock1->bUseTexturesAsMaterials = true;
	pRock1->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pRock1->vecTextureMixRatios[0] = 0.0f;		// Glass
	pRock1->vecTextureMixRatios[1] = 0.0f;		// Blue tiles
	pRock1->vecTextureMixRatios[2] = 0.0f;		// Gold leaf
	pRock1->vecTextureMixRatios[3] = 0.0f;		// Blue Whale
	pRock1->vecTextureMixRatios[4] = 1.0f;		//sand

	cGameObject* pRock2 = new cGameObject();
	pRock2->modelName = "assets/models/asteroid_sc0001.ply";
	pRock2->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pRock2->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pRock2->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pRock2->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pRock2->scale = 0.3f;
	pRock2->position.z = -5.0f;
	pRock2->position.x = 3.5f;
	pRock2->position.y = -5.0f;
	pRock2->bUseTexturesAsMaterials = true;
	pRock2->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pRock2->vecTextureMixRatios[0] = 0.0f;		// Glass
	pRock2->vecTextureMixRatios[1] = 0.0f;		// Blue tiles
	pRock2->vecTextureMixRatios[2] = 0.0f;		// Gold leaf
	pRock2->vecTextureMixRatios[3] = 0.0f;		// Blue Whale
	pRock2->vecTextureMixRatios[4] = 1.0f;		//sand

	cGameObject* pRock3 = new cGameObject();
	pRock3->modelName = "assets/models/asteroid_sc0001.ply";
	pRock3->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pRock3->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pRock3->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pRock3->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pRock3->scale = 0.15f;
	pRock3->position.z = -2.0f;
	pRock3->position.x = -3.5f;
	pRock3->position.y = -5.0f;
	pRock3->bUseTexturesAsMaterials = true;
	pRock3->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pRock3->vecTextureMixRatios[0] = 0.0f;		// Glass
	pRock3->vecTextureMixRatios[1] = 0.0f;		// Blue tiles
	pRock3->vecTextureMixRatios[2] = 0.0f;		// Gold leaf
	pRock3->vecTextureMixRatios[3] = 0.0f;		// Blue Whale
	pRock3->vecTextureMixRatios[4] = 1.0f;		//sand

	cGameObject* pPlant1 = new cGameObject();
	pPlant1->modelName = "assets/models/Plant1.ply";
	pPlant1->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pPlant1->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pPlant1->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pPlant1->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pPlant1->scale = 0.3f;
	pPlant1->position.z = -2.0f;
	pPlant1->position.x = 6.5f;
	pPlant1->position.y = -3.4f;
	pPlant1->bUseTexturesAsMaterials = true;
	pPlant1->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pPlant1->vecTextureMixRatios[0] = 0.0f;		// Glass
	pPlant1->vecTextureMixRatios[1] = 1.0f;		// Blue tiles
	pPlant1->vecTextureMixRatios[2] = 0.0f;		// Gold leaf
	pPlant1->vecTextureMixRatios[3] = 0.0f;		// Blue Whale
	pPlant1->vecTextureMixRatios[4] = 0.0f;

	cGameObject* pPlant2 = new cGameObject();
	pPlant2->modelName = "assets/models/Plant2.ply";
	pPlant2->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pPlant2->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pPlant2->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pPlant2->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pPlant2->scale = 3.3f;
	pPlant2->position.z = -2.0f;
	pPlant2->position.x = -3.5f;
	pPlant2->position.y = -5.0f;
	pPlant2->bUseTexturesAsMaterials = true;
	pPlant2->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pPlant2->vecTextureMixRatios[0] = 0.0f;		// Glass
	pPlant2->vecTextureMixRatios[1] = 1.0f;		// Blue tiles
	pPlant2->vecTextureMixRatios[2] = 0.0f;		// Gold leaf
	pPlant2->vecTextureMixRatios[3] = 0.0f;		// Blue Whale
	pPlant2->vecTextureMixRatios[4] = 0.0f;

	cGameObject* pTankFloor = new cGameObject();
	pTankFloor->modelName = "assets/models/tankGround.ply";
	pTankFloor->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pTankFloor->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pTankFloor->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pTankFloor->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pTankFloor->scale = 0.0082f;
	pTankFloor->position.z = -2.0f;
	pTankFloor->position.x = -1.6f;
	pTankFloor->position.y = -5.3f;
	pTankFloor->bUseTexturesAsMaterials = true;
	pTankFloor->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	pTankFloor->vecTextureMixRatios[2] = 1.0f;

	cGameObject* pTankFloor2 = new cGameObject();
	pTankFloor2->modelName = "assets/models/tankGround.ply";
	pTankFloor2->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pTankFloor2->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pTankFloor2->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pTankFloor2->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pTankFloor2->scale = 0.0082f;
	pTankFloor2->position.z = -2.0f;
	pTankFloor2->position.x = 8.0f;
	pTankFloor2->position.y = -5.3f;
	pTankFloor2->bUseTexturesAsMaterials = true;
	pTankFloor2->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	pTankFloor2->vecTextureMixRatios[2] = 1.0f;

	cGameObject* pTankGlass = new cGameObject();
	pTankGlass->modelName = "assets/models/tankGlass.ply";
	pTankGlass->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pTankGlass->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pTankGlass->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pTankGlass->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pTankGlass->scale = 1.0f;
	pTankGlass->position.z = -2.0f;
	pTankGlass->position.x = 3.5f;
		// UV coloured grid thing
	pTankGlass->bIsEntirelyTransparent = true;
	pTankGlass->alphaValue = 0.3f;


	cGameObject* pTankFrame = new cGameObject();
	pTankFrame->modelName = "assets/models/tankFrame.ply";
	pTankFrame->debugColour = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	pTankFrame->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pTankFrame->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pTankFrame->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pTankFrame->scale = 1.0f;
	pTankFrame->position.z = -2.0f;
	pTankFrame->position.x = 3.5f;



	cGameObject* pBlueWhale = new cGameObject();
	pBlueWhale->modelName = "assets/models/BlueWhale.ply";
	pBlueWhale->debugColour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	pBlueWhale->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pBlueWhale->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pBlueWhale->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pBlueWhale->scale = 20.0f;
	pBlueWhale->preRotation.x = glm::radians( +90.0f );
	pBlueWhale->preRotation.z = glm::radians(+40.0f);
	//pBlueWhale->preRotation.z = glm::radians( -135.0f );
//	pBlueWhale->position.z = 1.0f;
	pBlueWhale->position.z = -2.0f;
	pBlueWhale->position.x = 3.5f;
	pBlueWhale->shininess = 1.0f;

	pBlueWhale->bUseTexturesAsMaterials = true;
	//pBlueWhale->bUseTexturesWithNoLighting = true;
	// Set up 8 textures in the model
	pBlueWhale->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	// Use texture #0, don't mix with any others
	pBlueWhale->vecTextureMixRatios[0] = 0.0f;		// Glass
	pBlueWhale->vecTextureMixRatios[1] = 0.0f;		// Blue tiles
	pBlueWhale->vecTextureMixRatios[2] = 0.0f;		// Gold leaf
	pBlueWhale->vecTextureMixRatios[3] = 1.0f;		// Blue Whale
	pBlueWhale->vecTextureMixRatios[4] = 0.0f;		// UV coloured grid thing


	//cGameObject* pBunny = new cGameObject();
	//pBunny->modelName = "assets/models/bun_zipper_hiRez.ply";
	//pBunny->debugColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//pBunny->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//pBunny->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	//pBunny->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	//pBunny->shininess = 1000.0f;			// 1.0 to 100,000 
	//pBunny->scale = 50.0f;
	////pBunny->position.x = -3.0f;
	//pBunny->position = glm::vec3(-1.96708f, -3.57167f, -2.23921f);

	//pBunny->bUseDiscardMask = true;		// Assume mask is in texture unit 7


	//cGameObject* pSplat = new cGameObject();
	//pSplat->modelName = "assets/models/1x1_6_Cluster_2_Sided_xyz_nxyz_uv.ply";
	//pSplat->debugColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//pSplat->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//pSplat->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	//pSplat->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	//pSplat->shininess = 1000.0f;			// 1.0 to 100,000 
	//pSplat->scale = 3.0f;
	////pBunny->position.x = -3.0f;
	//pSplat->position = glm::vec3(0.0f, 3.0f, 2.0f);

	//pSplat->bUseDiscardMask = true;		// Assume mask is in texture unit 7
	//// To use the explosion texture, we need to set the "mix" ratios, too
	//pSplat->ClearTextureMixValues(NUMBEROF2DSAMPLERS, 0.0f);
	//// "Explosion" is texture unit #6 (see SetTextureBindings() function for this)
	//pSplat->vecTextureMixRatios[6] = 1.0f;	// All explosion, all the time...
	//pSplat->bUseTexturesAsMaterials = true;		// Tell the shader we want textures
	//pSplat->bUseTexturesWithNoLighting = true;	// And that we DON'T want lighting (as it's an explosion) 
	//                                            // - you could change this, if you had a light right in the 
	//                                            //   centre of the explosion, for instance
	::g_vec_pGOs.push_back(pTankFloor2);
	::g_vec_pGOs.push_back(pTankFloor);

	::g_vec_pGOs.push_back( pBlueWhale );
	::g_vec_pGOs.push_back(pPlant1);
	::g_vec_pGOs.push_back(pPlant2);
	::g_vec_pGOs.push_back(pRock1);
	::g_vec_pGOs.push_back(pRock2);
	::g_vec_pGOs.push_back(pRock3);
	::g_vec_pGOs.push_back(pFish1);
	::g_vec_pGOs.push_back(pFish2);
	::g_vec_pGOs.push_back(pFish3);
	::g_vec_pGOs.push_back(pFish4);
	::g_vec_pGOs.push_back(pFish5);
	::g_vec_pGOs.push_back(pFish6);
	::g_vec_pGOs.push_back(pFish7);
	::g_vec_pGOs.push_back(pFish8);
	::g_vec_pGOs.push_back(pFish9);
	::g_vec_pGOs.push_back(pFish10);
	::g_vec_pGOs.push_back(pFish11);

	::g_vec_pGOs.push_back(pCastle);

	::g_vec_pGOs.push_back(pTankFrame);
	::g_vec_pGOs.push_back(pTankGlass);

	return;
}
//#ifndef UTILS_H
//#define UTILS_H
// Was called Utils.h, but is really sort of a "global" file now
#ifndef _globals_HG_
#define _globals_HG_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
//#define FREEGLUT_STATIC
//#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/freeglut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>		// Basic types
#include <glm/gtc/matrix_transform.hpp>		// Rot, tran
#include <glm/gtc/type_ptr.hpp>	// Passing up to OpenGL


#include "cGameObject.h"
#include <vector>


#include "CHRTimer.h"

#include "CShaderManager/IGLShaderManager.h"		// Note: it's "I" (for "interface") here
#include "GLTexture/CTextureManager.h"

#include "cLightDesc.h"

// All of our game objects
extern std::vector< cGameObject* > g_vec_pGOs;

extern IGLShaderManager* g_pTheShaderManager;

extern CTextureManager* g_pTheTextureManager;

extern glm::vec3 g_cam_eye;
extern glm::vec3 g_cam_at;

extern CHRTimer g_AniTimer;		// 

extern cGameObject* g_pDebugBall;

static const double PI = 3.14159265358979323846;

static const int NUMBEROFLIGHTS = 10;
extern std::vector<cLightDesc> g_vecLights;

extern int g_selectedLightIndex;

extern bool g_bDebugLights;

//typedef struct Vertex
//{
//  float Position[4];		// 4 floats * 4 = 16 bytes
//  float Color[4];			// 4 floats * 4 = 16 bytes
//} Vertex;

//layout(location=0) in vec4 in_Position;
//layout(location=1) in vec4 in_Normal;	// Color is being used
//layout(location=2) in vec4 in_RGBA;
//layout(location=3) in vec4 in_UV_x2;

typedef struct Vertex_xyz_n_RGB_UVx2
{
  float Position[4];		// 4 floats * 4 = 16 bytes
  float Normal[4];			// 4 floats * 4 = 16 bytes
  float RGBA[4];
  float UVx2[4];
} Vertex_xyz_n_RGB_UVx2;

//typedef struct Matrix
//{ 
//  float m[16];
//} Matrix;

//extern const Matrix IDENTITY_MATRIX;

float Cotangent(float angle);
float DegreesToRadians(float degrees);
float RadiansToDegrees(float radians);

//Matrix MultiplyMatrices(const Matrix* m1, const Matrix* m2);
//void RotateAboutX(Matrix* m, float angle);
//void RotateAboutY(Matrix* m, float angle);
//void RotateAboutZ(Matrix* m, float angle);
//void ScaleMatrix(Matrix* m, float x, float y, float z);
//void TranslateMatrix(Matrix* m, float x, float y, float z);

//Matrix CreateProjectionMatrix(
//  float fovy,
//  float aspect_ratio,
//  float near_plane,
//  float far_plane
//);

void ExitOnGLError(const char* error_message);
GLuint LoadShader(const char* filename, GLenum shader_type);



#endif
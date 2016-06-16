#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <glm/glm.hpp>	// 
#include <string>

#include <vector>

// To simplify, this is also a "mesh"
class cGameObject
{
public:
	cGameObject(); /*: 
		cached_VBO_ID(-1), 
		scale(1.0f),
		bUseDebugColour(false),
		bIsWireframe(false), 
		shininess(1.0f), 
		bIsADebugObject(false)
	{};*/
	glm::vec3 position;
	glm::vec3 preRotation;
	glm::vec3 postRotation;
	float scale;	
	//glm::vec4 colour;
	glm::vec4 debugColour;
	bool bUseDebugColour;
	bool bIsWireframe;

	bool bIsVisible;

	// For transparency
	bool bIsEntirelyTransparent;
	float alphaValue;

	bool bUseDiscardMask;

	glm::vec4 diffuse;
	glm::vec4 ambient;
	glm::vec3 specular;
	float shininess;

	// Added
	std::string modelName;		// File name
	int cached_VBO_ID;// = -1;	// Huh??
	unsigned int numberOfTriangles;

	bool bIsADebugObject;

	bool bUseVertexRGBAColoursAsMaterials;

	bool bUseTexturesAsMaterials;
	bool bUseTexturesWithNoLighting;
	// How much of each texture is combined for the final texture
	// The index value indicates which sampler we're talking about
	std::vector<float> vecTextureMixRatios;
	void ClearTextureMixValues(int numberOfSamplers, float defaultMixValue);

	// More physics things
	glm::vec3 velocity;
	glm::vec3 accel;		// acceleration
};

#endif

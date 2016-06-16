#ifndef _cLightDesc_HG_
#define _cLightDesc_HG_

// theLights[x].position
#include <glm/glm.hpp>		// Basic types

struct cLightDesc
{
	cLightDesc();
	glm::vec4 position;
	glm::vec4 ambient;	
	glm::vec4 diffuse;
	glm::vec4 specular;
	float attenConst;	// = 0.1f;
	float attenLinear;	// = 0.1f;
	float attenQuad;	// = 0.1f;
	int lightType;   // 0=point, 1=spot, 2=directional
	// Or something like this
	//enum enumLightTypes
	//{	
	//	POINT = 0,
	//	SPOT = 1,
	//	DIRECTIONAL = 2
	//};
	//enumLightTypes lightType;

	// For spot and directional
	glm::vec4 direction;
	// For spot
	float anglePenumbraStart;
	float anglePenumbraEnd;

	// Nasty OpenGL stuff 
	int UniLoc_position;		// GLint
	int UniLoc_ambient;
	int UniLoc_diffuse;
	int UniLoc_specular;
	int UniLoc_attenConst;
	int UniLoc_attenLinear;
	int UniLoc_attenQuad;
	int UniLoc_type;
	int UniLoc_direction;
	int UniLoc_anglePenumbraStart;
	int UniLoc_anglePenumbraEnd;

	// 0.25 passed in, returns some distance
	float calcDistanceAtBrightness( float brightness );

	float calcAttenuation(float dist);

};


#endif

#include "cGameObject.h"

cGameObject::cGameObject()
{
	this->cached_VBO_ID = -1;	
	this->scale = 1.0f;

	this->bUseDebugColour = false;
	this->bIsWireframe = false;

	this->shininess = 1.0f;

	this->bIsADebugObject = false;

	this->bUseTexturesAsMaterials = false;
	this->bUseTexturesWithNoLighting = false;

	this->bUseVertexRGBAColoursAsMaterials = false;
	
	this->bIsVisible = true;

	// This to make the object transparent entirely and uniformly
	this->bIsEntirelyTransparent = false;
	this->alphaValue = 1.0f;	// NOT transparent

	this->bUseDiscardMask = false;

	return;
}

void cGameObject::ClearTextureMixValues(int numberOfSamplers, float defaultMixValue)
{
	this->vecTextureMixRatios = std::vector<float>(numberOfSamplers, defaultMixValue);
	return;
}



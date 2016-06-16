#include "cLightDesc.h"

cLightDesc::cLightDesc()
{
	this->attenConst = 0.0f;
	this->attenLinear = 0.1f;
	this->attenQuad = 0.1f;		// Why not?

	this->lightType = 0;	

	this->UniLoc_position = -1;
	this->UniLoc_ambient = -1;
	this->UniLoc_diffuse = -1;
	this->UniLoc_attenConst = -1;
	this->UniLoc_attenLinear = -1;
	this->UniLoc_attenQuad = -1;
	this->UniLoc_specular = -1;
	this->UniLoc_direction = -1;
	this->UniLoc_anglePenumbraStart = -1;
	this->UniLoc_anglePenumbraEnd = -1;
	this->UniLoc_type = -1;

	return;
}

// 0.25 passed in, returns some distance
float cLightDesc::calcDistanceAtBrightness( float brightness )	// 0.5f;
{
	const float epsilon = 0.01f;
	// 6 million times a second... 
	for ( float x = 0.0f; x < 50.0f; x += 0.0001f )			// 5000x
	{
		float curAtten = calcAttenuation(x);

		// is it within a certain range of values
		if ( curAtten >= (brightness-epsilon) && 		// 0.5-0.01, 4.99f
			 curAtten <= (brightness+epsilon) )         // 0.5+0.01, 5.01f
		{
			return x;
		}
	}
	return 0.0f;
}



float cLightDesc::calcAttenuation(float dist)
{
	// Taken right from the shader
	//float atten = 1.0f /( theLights[lightID].attenConst + 
	//                      theLights[lightID].attenLinear * dist +
	//					  theLights[lightID].attenQuad * dist * dist );
	float atten = 1.0f /( this->attenConst + 
	                      this->attenLinear * dist +
						  this->attenQuad * dist * dist );

	return atten;
}
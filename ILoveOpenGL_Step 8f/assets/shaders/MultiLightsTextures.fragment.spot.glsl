#version 400

//in vec4 ex_Color;
in vec4 ex_Position;		// Coming from vertex
in vec4 ex_PositionWorld;
in vec4 ex_Normal;			// Coming from vertex
in vec4 ex_RGBA;
in vec4 ex_UV_x2;

out vec4 out_Colour;

struct LightDesc
{
	vec4 position;
	vec4 ambient;	
	vec4 diffuse;
	vec4 specular;
	float attenConst;	// = 0.1f;
	float attenLinear;	// = 0.1f;
	float attenQuad;	// = 0.1f;
	//
	float lightType;   // 0.0=point, 1.0=spot, 2.0=directional
	vec4 direction;
	float anglePenumbraStart;
	float anglePenumbraEnd;
};


uniform LightDesc oneLonelyLight;

const int NUMLIGHTS = 10;
uniform LightDesc theLights[NUMLIGHTS];

// If you want to make faster unifrom calls, check into "named uniform block"
//layout(std140) uniform LightingBlock
//{
//	LightDesc Light[NUMLIGHTS];
//} theLights;

// For directly setting the ambient and diffuse (if NOT using textures)
uniform vec3 myMaterialAmbient_RGB;		// = vec3( 0.2f , 0.1f, 0.0f );
uniform vec3 myMaterialDiffuse_RGB;		// = vec3( 1.0f , 0.5f, 0.0f );
uniform vec3 myMaterialSpecular;	// = vec3( 0.6f, 0.6f, 0.6f );
uniform float myMaterialShininess;	// = 80.0f; 

uniform bool bAlphaForEntireObject; 	// True, then set global alpha
uniform float myAlphaAllObject;		// 0.0f to 1.0f

// Discard transparency
uniform bool bUseDiscardMask; 

uniform vec3 eye;	// Eye location of the camera

uniform vec4 debugColour;	
uniform bool bUseDebugColour;	// Note: the "bool" type is relatively new to shaders. 
                                // You could also use a float or int (0.0 being false, anything else being true, for instance)
uniform bool bUseVertexRGBAColours;	// Uses the vertex values rather than the texture or overall
uniform bool bUseTextureMaterials;	
uniform bool bUseTexturesNoLighting;		// Does not use lighting
	
// Our 8 2D samplers (which is a lot, considering what we know at this point)	
const int NUMBEROFSAMPLERS = 8;	// Used later in the shader							
uniform sampler2D texSamp2D_00;		// Texture unit 0 (GL_TEXTURE0)
uniform sampler2D texSamp2D_01;		// GL_SAMPLER_2D
uniform sampler2D texSamp2D_02;		
uniform sampler2D texSamp2D_03;
uniform sampler2D texSamp2D_04;
uniform sampler2D texSamp2D_05;
uniform sampler2D texSamp2D_06;
uniform sampler2D texSamp2D_07;				// Transparency layer

uniform float textureMixRatios[NUMBEROFSAMPLERS];


				
vec3 ADSLightModelPoint( in vec3 myNormal, in vec3 myPosition, 
                         in vec3 matDiffuse, in vec3 matAmbient, in vec3 matSpec,
                         in int lightID );
vec3 ADSLightModelSpot( in vec3 myNormal, in vec3 myPosition, 
                        in vec3 matDiffuse, in vec3 matAmbient, in vec3 matSpec,
                        in int lightID );
vec3 ADSLightModelDirectional( in vec3 myNormal, in vec3 myPosition, 
                               in vec3 matDiffuse, in vec3 matAmbient, in vec3 matSpec,
                               in int lightID );
		

vec4 correctForGamma( vec4 colour )
{
	//From: Gerdelan, Anton (2014-06-25). Anton's OpenGL 4 Tutorials (Kindle Locations 1935-1936).
	// He talks about modern (flatscreen, I'm assuming) monitors having a standard gamma of 2.2
	return vec4( pow(colour.rgb, vec3 (1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2)), colour.a ) ;
	//return colour;
}

void main(void)
{
	out_Colour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	
	
	// ex_Normal came from the vertex shader.
	//	it's the Normal at this particular pixel fragment
	vec3 myNormal = vec3( ex_Normal.x, 
						  ex_Normal.y, 
						  ex_Normal.z );
						
	vec3 myPositionWorld = vec3( ex_PositionWorld.xyz );
	
		// Are we using the "debug" colour (i.e. no lighthing)
	if ( bUseDebugColour )
	{	// Yes, so skip lighting and texturing
		out_Colour = debugColour;
		// Exit early (it's faster)
		correctForGamma(out_Colour);
		return; //**EARLY EXIT**//
	}

	vec3 lightContrib = vec3(0.0f, 0.0f, 0.0f);
	// Could optionally add the texture samples as the "colour" 
	// 	of the vertex at this position
	for ( int LightIndex = 0; LightIndex < NUMLIGHTS; LightIndex++ )
	{
//		//int LightIndex = 0;
//		lightContrib += ADSLightModel( myNormal, myPositionWorld,
//		                         vec3(1.0f,1.0f,1.0f), 	// Material Diffuse
//								 vec3(0.2f,0.2f,0.2f),	// Material Ambient
//								 myMaterialSpecular,
//                               LightIndex ); 

		// 0.0=point, 1.0=spot, 1984.0=directional
								 
		switch ( int(theLights[LightIndex].lightType) )
		{
		case 2:		// Directional
			lightContrib += ADSLightModelDirectional( myNormal, myPositionWorld,
			                                          vec3(1.0f,1.0f,1.0f), 	// Material Diffuse
			                                          vec3(0.2f,0.2f,0.2f),	// Material Ambient
			                                          myMaterialSpecular,
			                                          LightIndex ); 
			break;
		case 1:		// Spot
			lightContrib += ADSLightModelSpot( myNormal, myPositionWorld,
			                                   vec3(1.0f,1.0f,1.0f), 	// Material Diffuse
			                                   vec3(0.2f,0.2f,0.2f),	// Material Ambient
			                                   myMaterialSpecular,
			                                   LightIndex ); 
			break;
		case 0:		// Point
		default:
			lightContrib += ADSLightModelPoint( myNormal, myPositionWorld,
			                                    vec3(1.0f,1.0f,1.0f), 	// Material Diffuse
			                                    vec3(0.2f,0.2f,0.2f),	// Material Ambient
			                                    myMaterialSpecular,
			                                    LightIndex ); 
			break;	
		}
	}
	
//	lightContrib *= 0.1f;
//	
//	switch ( int(theLights[0].lightType) )
//	{
//	case 2:		// Directional
//		lightContrib.b = 1.0f;
//		break;
//	case 1:		// Spot
//		lightContrib.g = 1.0f;
//		break;
//	case 0:		// Point
//	default:
//		lightContrib.r = 1.0f; 
//		break;	
//	}
	
	// Limit the colour range from 0.0 to 1.0
    lightContrib = clamp( lightContrib, 0.0f, 1.0f );
		

	// Lighting + vertex RGB values (assume ambient is 0.2f diffuse)
	if ( bUseVertexRGBAColours )
	{	// Use the vertex values for colour
		vec3 colourDiffuse = ex_RGBA.rgb * lightContrib;
		vec3 colourAmbient = colourDiffuse * 0.2f;
		vec3 colour = clamp( colourDiffuse + colourAmbient, 0.0f, 1.0f );
		out_Colour = vec4( colour, 1.0f );
	}
	else if ( bUseTextureMaterials || bUseTexturesNoLighting )
	{	// Use textures as diffuse (and ambient) values
		// TODO: This does NOT work as written. We'll be talking about textures soon enough, though

		vec3 texColours[NUMBEROFSAMPLERS];

		// Getting all these samples is pretty silly, but it's to demonstrate a few things:
		// * If we don't USE the sampler, then the uniform will be removed
		//   (this will likely cause issues in the C++ side of things, when 
		//    we call glUniform() on something that's not there
		// * On modern GPUs, sampling textures is very inexpensive
		// * There is a cost to "switching" samplers. On modern GPUs, 
		//   this isn't prohibitive, but there -is- a cost, likely 
		//   more expensive than sampling nothing. But you'd have to decide
		//   what's easier: managing the 'switching' of textures in and out,
		//   or not using textures that aren't there. 
		texColours[0] = texture(texSamp2D_00, ex_UV_x2.xy).rgb;
		texColours[1] = texture(texSamp2D_01, ex_UV_x2.xy).rgb;
		texColours[2] = texture(texSamp2D_02, ex_UV_x2.xy).rgb;
		texColours[3] = texture(texSamp2D_03, ex_UV_x2.xy).rgb;
		texColours[4] = texture(texSamp2D_04, ex_UV_x2.xy).rgb;
//		texColours[4] = texture(texSamp2D_04, ex_UV_x2.xy).rgb * 0.01f;
//		texColours[4].rgb += vec3(ex_UV_x2.xy, 0.0f);
//		texColours[4].r += 1.0f;
		texColours[5] = texture(texSamp2D_05, ex_UV_x2.xy).rgb;
		texColours[6] = texture(texSamp2D_06, ex_UV_x2.xy).rgb;
		texColours[7] = texture(texSamp2D_07, ex_UV_x2.xy).rgb;

		
		vec3 texColour = vec3(0.0f, 0.0f, 0.0f);
		// Now we combine the textures. There's trade-offs here, of course... 
		for ( int index = 0; index < NUMBEROFSAMPLERS; index++ )
		{
			texColour += (textureMixRatios[index] * texColours[index]);	// additive, so will saturate
			
			// Note that if we multiply ("modulate"), the texture gets darker and darker...
			//texColour *= (textureMixRatios[index] * texColours[index]);	// modulate, so will bend down to black
			// So you may have to multiply by some known factor, like x8 in our case
		}
			
		vec3 colour = vec3(1.0f,1.0f,1.0f);
		
		if ( bUseTexturesNoLighting )
		{	// Ignore lighting component
			colour = clamp( texColour.xyz, 0.0f, 1.0f );
		}
		else
		{	// Apply the lighting
			vec3 texDiffuse = texColour.xyz;
			vec3 texAmbient = texDiffuse * 0.2f;
			colour = clamp( (lightContrib * texDiffuse) + texAmbient, 0.0f, 1.0f );
		}
		
		out_Colour = vec4( colour, 1.0f ); 				
	}
	else
	{	// Use the 'global' uniform diffuse and ambient values
		vec3 colourDiffuse = myMaterialDiffuse_RGB.xyz * lightContrib;
		vec3 colour = colourDiffuse + myMaterialAmbient_RGB.zyx;
		colour = clamp( colour, 0.0f, 1.0f );
		out_Colour = vec4( colour, 1.0f );	
		
		// TODO
	}//if ( bUseVertexRGBAColours )

	// rgb a		0.0 to 1.0f;
	//out_Colour.a = 0.0f;
	// Pass the alpha value for the object.
	
	if ( bAlphaForEntireObject )
	{	// Set alpha for entire object... 
		out_Colour.a = myAlphaAllObject;
	}
	
//	if ( bAlphaTextureMask )
//	{	// Set alpha for entire object... 
//		vec3 maskPixel = texture(texSamp2D_07, ex_UV_x2.xy).rgb;
//		float averageHue = (maskPixel.r + maskPixel.g + maskPixel.b) / 3.0f;
//		out_Colour.a = averageHue;
//	}
	
	// Discard transparency... oh yeah, baby
	if ( bUseDiscardMask )
	{	// Assume that this is loaded into texture #7		
		
		vec3 maskPixel = texture(texSamp2D_07, ex_UV_x2.xy).rgb;
		float averageHue = (maskPixel.r + maskPixel.g + maskPixel.b) / 3.0f;
		if ( averageHue < 0.5f )	
		{	// I'm outta here. Do NOT write the pixel
			discard;
		}		
	}
	
	
	correctForGamma(out_Colour);	
	return;
}

// Modified from the AMAZING
// Graphics Shaders: Theory and Practice, by Mike Bailey and Steve Cunningham
vec3 ADSLightModelPoint( in vec3 myNormal, in vec3 myPosition, 
                         in vec3 matDiffuse, in vec3 matAmbient, in vec3 matSpec,
                         in int lightID )
{
	// normal, light, view, and light reflection vectors
	vec3 norm = normalize( myNormal );
	vec3 lightv = normalize( theLights[lightID].position.xyz - myPosition);
	

	vec3 viewv = normalize( eye - myPosition );
	//vec3 refl = reflect( eye - lightv, norm );
	vec3 refl = -( reflect( lightv, norm ) );

	
	// ambient light computation
	vec3 ambient = matAmbient * theLights[lightID].ambient.rgb;
	
	// diffuse light computation
	vec3 diffuse = max(0.0f, dot(lightv, norm))
                   * matDiffuse
				   * theLights[lightID].diffuse.rgb;
				   
	float dist = distance( myPosition, theLights[lightID].position.xyz );	
	
	float atten = 1.0f /( theLights[lightID].attenConst + 
	                      theLights[lightID].attenLinear * dist +
						  theLights[lightID].attenQuad * dist * dist );
	
	diffuse *= atten;
	
	// specular light computation
	vec3 specular = vec3( 0.0f, 0.0f, 0.0f );	

	//if( dot(lightv, viewv) > 0.0f )
	if( dot(lightv, norm) > 0.0f )
	{
		specular = pow( max(0.0f, dot(viewv,refl)), myMaterialShininess)
					* matSpec
					* theLights[lightID].specular.rgb;// // myLightSpecular;
					//* myLights[LightIndex].specular.rgb;// // myLightSpecular;
		specular *= atten;
	}	

	return clamp( ambient + diffuse + specular, 0.0f, 1.0f );
}

vec3 ADSLightModelSpot( in vec3 myNormal, in vec3 myPosition, 
                        in vec3 matDiffuse, in vec3 matAmbient, in vec3 matSpec,
                        in int lightID )
{	
	// normal, light, view, and light reflection vectors
	vec3 norm = normalize( myNormal );
	vec3 lightv = normalize( theLights[lightID].position.xyz - myPosition);
	

	vec3 viewv = normalize( eye - myPosition );
	//vec3 refl = reflect( eye - lightv, norm );
	vec3 refl = -( reflect( lightv, norm ) );

	
	float spotAttenuation = 1.0f;
	// Normalized cone direction
	vec3 coneDirection = normalize(theLights[lightID].direction.xyz);
	// Light direction from vertex
	vec3 rayDirection = -lightv;
	// Angle between vertex-and-light and cone
	float lightToSurfaceAngle = degrees(acos(dot(rayDirection, coneDirection)));
	// Calculate the penumbra
	float penumbraAngleStart = theLights[lightID].anglePenumbraStart;
	float penumbraAngleEnd = theLights[lightID].anglePenumbraEnd;
	
	if ( penumbraAngleStart < 0.0f ) 	{ penumbraAngleStart = 0.0f; }
	if ( penumbraAngleEnd > 90.0f )		{ penumbraAngleEnd = 90.0f; }
	if ( penumbraAngleEnd < penumbraAngleStart )
	{	// What are you doing, man? Geepers... Start should be less than End... 
		float temp = penumbraAngleStart;
		penumbraAngleStart = penumbraAngleEnd;
		penumbraAngleEnd = temp;
	}
		
	if ( lightToSurfaceAngle > penumbraAngleEnd )
	{	// Outside of cone, so dark
		spotAttenuation = 0.0f;
	}
	else if ( lightToSurfaceAngle > penumbraAngleStart )
	{	// Is inside the penumbra range...
		spotAttenuation = smoothstep( penumbraAngleEnd, penumbraAngleStart, lightToSurfaceAngle );
	}
	//else ... vertex is inside light cone
	// spotAttenuation = 1.0f; (which it already is, yo)

	
	// ambient light computation
	vec3 ambient = matAmbient * theLights[lightID].ambient.rgb;
	
	// diffuse light computation
	vec3 diffuse = max(0.0f, dot(lightv, norm))
                   * matDiffuse
				   * theLights[lightID].diffuse.rgb;
				   
	float dist = distance( myPosition, theLights[lightID].position.xyz );	
	
	float atten = 1.0f /( theLights[lightID].attenConst + 
	                      theLights[lightID].attenLinear * dist +
						  theLights[lightID].attenQuad * dist * dist );
	
	diffuse *= atten;
	
	// specular light computation
	vec3 specular = vec3( 0.0f, 0.0f, 0.0f );	

	//if( dot(lightv, viewv) > 0.0f )
	if( dot(lightv, norm) > 0.0f )
	{
		specular = pow( max(0.0f, dot(viewv,refl)), myMaterialShininess)
					* matSpec
					* theLights[lightID].specular.rgb;// // myLightSpecular;
					//* myLights[LightIndex].specular.rgb;// // myLightSpecular;
		specular *= atten;
	}	

	return clamp( ambient + spotAttenuation*(diffuse + specular), 0.0f, 1.0f );
	//return clamp( ambient + 1.0f*(diffuse + specular), 0.0f, 1.0f );
}

// Does NOT work as implemented... 
// Instead, returns a random colour
vec3 ADSLightModelDirectional( in vec3 myNormal, in vec3 myPosition, 
                               in vec3 matDiffuse, in vec3 matAmbient, in vec3 matSpec,
                               in int lightID )
{
	// Return a pseudo-random number
	// From: http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
    return vec3( fract(sin(dot(myNormal.xy ,vec2(12.9898,78.233))) * 43758.5453), 
				 fract(sin(dot(myNormal.yz ,vec2(12.9898,78.233))) * 43758.5453),
				 fract(sin(dot(myNormal.zx ,vec2(12.9898,78.233))) * 43758.5453) );
	//return vec3(0.0f, 1.0f, 0.0f);

	// normal, light, view, and light reflection vectors
	vec3 norm = normalize( myNormal );
	vec3 lightv = normalize( theLights[lightID].position.xyz - myPosition);
	

	vec3 viewv = normalize( eye - myPosition );
	//vec3 refl = reflect( eye - lightv, norm );
	vec3 refl = -( reflect( lightv, norm ) );

	
	// ambient light computation
	vec3 ambient = matAmbient * theLights[lightID].ambient.rgb;
	
	// diffuse light computation
	vec3 diffuse = max(0.0f, dot(lightv, norm))
                   * matDiffuse
				   * theLights[lightID].diffuse.rgb;
				   
	float dist = distance( myPosition, theLights[lightID].position.xyz );	
	
	float atten = 1.0f /( theLights[lightID].attenConst + 
	                      theLights[lightID].attenLinear * dist +
						  theLights[lightID].attenQuad * dist * dist );
	
	diffuse *= atten;
	
	// specular light computation
	vec3 specular = vec3( 0.0f, 0.0f, 0.0f );	

	//if( dot(lightv, viewv) > 0.0f )
	if( dot(lightv, norm) > 0.0f )
	{
		specular = pow( max(0.0f, dot(viewv,refl)), myMaterialShininess)
					* matSpec
					* theLights[lightID].specular.rgb;// // myLightSpecular;
					//* myLights[LightIndex].specular.rgb;// // myLightSpecular;
		specular *= atten;
	}	

	return clamp( ambient + diffuse + specular, 0.0f, 1.0f );
}







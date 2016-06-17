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
	int lightType;   // 0=point, 1=spot, 2=directional
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

uniform vec3 eye;	// Eye location of the camera

uniform vec4 debugColour;	
uniform bool bUseDebugColour;	// Note: the "bool" type is relatively new to shaders. 
                                // You could also use a float or int (0.0 being false, anything else being true, for instance)
uniform bool bUseVertexRGBAColours;	// Uses the vertex values rather than the texture or overall
uniform bool bUseTextureMaterials;	
	
// Our 8 2D samplers (which is a lot, considering what we know at this point)								
uniform sampler2D texSamp2D_00;		// Texture unit 0 (GL_TEXTURE0)
uniform sampler2D texSamp2D_01;		// GL_SAMPLER_2D
uniform sampler2D texSamp2D_02;		
uniform sampler2D texSamp2D_03;
uniform sampler2D texSamp2D_04;
uniform sampler2D texSamp2D_05;
uniform sampler2D texSamp2D_06;
uniform sampler2D texSamp2D_07;	
uniform sampler2D texSamp2D_08;
uniform sampler2D texSamp2D_09;
uniform sampler2D texSamp2D_10;	
uniform sampler2D texSamp2D_11;					
								
vec3 ADSLightModel( in vec3 myNormal, in vec3 myPosition, 
                    in vec3 matDiffuse, in vec3 matAmbient, in vec3 matSpec,
                    in int lightID );
		

vec4 correctForGamma( vec4 colour )
{
	//From: Gerdelan, Anton (2014-06-25). Anton's OpenGL 4 Tutorials (Kindle Locations 1935-1936).
	// He talks about modern (flatscreen, I'm assuming) monitors having a standard gamma of 2.2
	vec4 corrected = vec4( pow(colour.rgb, vec3 (1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2)), colour.a ) ;
	corrected *= 1.2f;
	corrected.a = 1.0f;
	return corrected;
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
		//int LightIndex = 0;
		lightContrib += ADSLightModel( myNormal, myPositionWorld,
		                         vec3(1.0f,1.0f,1.0f), 	// Material Diffuse
								 vec3(0.2f,0.2f,0.2f),	// Material Ambient
								 myMaterialSpecular,
                                 LightIndex ); 
	}
	
	// Limit the colour range from 0.0 to 1.0
    lightContrib = clamp( lightContrib, 0.0f, 1.0f );
		

	// Lighting + vertex RGB values (assume ambient is 0.2f diffuse)
	if ( bUseVertexRGBAColours )
	{	// Use the vertex values for colour
		vec3 colourDiffuse = ex_RGBA.xyz * lightContrib;
		vec3 colourAmbient = colourDiffuse * 0.2f;
		vec3 colour = clamp( colourDiffuse + colourAmbient, 0.0f, 1.0f );
		out_Colour = vec4( ex_RGBA.xyz, 1.0f );
	}
	else if ( bUseTextureMaterials )
	{	// Use textures as diffuse (and ambient) values
		// TODO: This does NOT work as written. We'll be talking about textures soon enough, though
		vec4 texture0Colour = texture( texSamp2D_00, ex_UV_x2.xy );	
		//vec4 texture1Colour = sampleColour = texture( texSamp2D_01, UV.zw );	
		//vec4 texture2Colour = sampleFromTexture2D( tex2DSelectX4.z, ex_UV_x2.zw );	
		//vec4 texture3Colour = sampleFromTexture2D( tex2DSelectX4.w, ex_UV_x2.zw );	
		//vec3 texDiffuse = (texture0Colour * texture1Colour * texture2Colour * texture3Colour).xyz;
		vec3 texDiffuse = texture0Colour.xyz;
		vec3 texAmbient = texDiffuse * 0.2f;
		// 
		vec3 colour = clamp( (lightContrib * texDiffuse) + texAmbient, 0.0f, 1.0f );
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

	
//	out_Colour.rgb = vec3( 1.0f, 0.0f, 0.0f); //myMaterialDiffuse_RGB.rgb;
	
	correctForGamma(out_Colour);	
	return;
}

// Modified from the AMAZING
// Graphics Shaders: Theory and Practice, by Mike Bailey and Steve Cunningham
vec3 ADSLightModel( in vec3 myNormal, in vec3 myPosition, 
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




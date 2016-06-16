#version 400

// Pixel Fragment shader 

in vec4 ex_Color;
out vec4 out_Color;

// Added
uniform vec4 myColour;

void main(void)
{
	// Clamp but don't remove
	out_Color = ex_Color * 0.0001f;

	out_Color += myColour;
}
// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Modified by:
//			Adrian Bathan, University of Calgary (30011953)
// Date:    December 2015
// Modified on: February 8, 2018
// ==========================================================================
#version 410

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec2 VertexPosition;
layout(location = 1) in vec3 VertexColour;
layout(location = 2) in vec2 TexturePos;


// output to be interpolated between vertices and passed to the fragment stage
out vec3 Colour;
out vec2 UV;
uniform float xoffset;
uniform float yoffset;
uniform float velocity;
uniform int scene;
uniform int mode;
uniform int level;
//uniform int level;
//uniform int fill;

void main()
{
    // assign vertex position without modification
//    if (fill != 1)
	if (scene == 3) {
		gl_Position = vec4((VertexPosition+vec2(xoffset,yoffset))/2-vec2(1,0)+vec2(-velocity,0), 0.0, 1.0);
		Colour = VertexColour;
	}
	else {
		if (mode == 0) 
			gl_Position = vec4(VertexPosition/2.5, 0.0, 1.0);
		else 
			gl_Position = vec4((VertexPosition-5)/5.5+vec2(0.1,0.4), 0.0, 1.0);
//		Colour = vec3(1.0f,0.0f,0.0f);
//		if (level >1)
//			Colour = vec3(0.0f,1.0f,0.0f);
//		if (level ==2)
//		else
//			Colour = vec3(1.0f,1.0f,1.0f);
	}
	Colour = VertexColour;
//	else
//		gl_Position = vec4(TexturePos, 0.0, 1.0);

    // assign output colour to be interpolated
    
//    tcPos = VertexPosition;
    UV=TexturePos;
}

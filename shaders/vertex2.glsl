// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Co-Authors:
//			Jeremy Hart, University of Calgary
//			John Hall, University of Calgary
// Modified by:
//			Adrian Bathan, University of Calgary (30011953)
// Date:    December 2015
// Modified on: February 28, 2018
// Citation:
//		https://medium.com/@evanwallace/easy-scalable-text-rendering-on-the-gpu-c3f4d782c5ac
//		https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch25.html
//		https://www.fontsquirrel.com/fonts/list/find_fonts?filter%5Btags%5D%5B0%5D=decorative&filter%5Btags%5D%5B1%5D=elegant&sort=hot
//		https://www.martinstoeckli.ch/fontmap/fontmap.html
//		http://www.shaderific.com/glsl-functions/
//		https://en.wikibooks.org/wiki/GLSL_Programming/GLUT/Transparency
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
uniform int pen;
uniform float zoom;
uniform mat4 perspective;
//uniform int level;
//uniform int fill;
#define PI 3.141592653589793238462643383279502884197169

void main()
{
    // assign vertex position without modification
	if (scene == 3) {
		gl_Position = vec4((((VertexPosition+vec2(xoffset,yoffset+2)-vec2(zoom/2,0))/2+vec2(0,velocity))*.16f), 0.0, 1.0);
//		gl_Position = vec4(gl_Position.xyz*vec3(1.0f,1.0f,sqrt(2.0f)),1.0);
//		gl_Position = vec4(vec3(gl_Position.x,gl_Position.y*sqrt(2.0f)/2.0f-gl_Position.z*sqrt(2.0f)/2.0f,gl_Position.y*sqrt(2.0f)/2.0f+gl_Position.z*sqrt(2.0f)/2.0f), 1.0);
		float scale=(gl_Position.y)/-2+1.6;
		gl_Position.xy*=scale;
//		gl_Position*=perspective;
//		gl_Position = gl_Position*perspective;
		
		Colour = VertexColour;
	}
//	else if (scene == 2)
	else if (scene ==0) {
		if (mode == 0) 
			gl_Position = vec4(VertexPosition/2.5, 0.0, 1.0);
		else 
			gl_Position = vec4((VertexPosition-5)/5.5+vec2(0.1,0.4), 0.0, 1.0);
		Colour = VertexColour;
	}
	else {
		gl_Position = vec4(VertexPosition, 0.0, 1.0);
		if (pen == 0)
			Colour = vec3(0.0f, 0.0f, 1.0f);
		else
			Colour = vec3(0.0f, 1.0f, 0.0f);
//		Colour = texture(ourTexture,TexturePos);
	}
	
    UV=TexturePos;
}

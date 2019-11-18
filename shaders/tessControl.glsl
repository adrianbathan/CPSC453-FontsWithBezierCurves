// ==========================================================================
// Tesscontrol program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
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

layout(vertices=4) out;
//layout(vertices=3) out;
//layout(triangles) out;

in vec3 tcColour[];
out vec3 teColour[];

//in gl_in[];		//Struct containing gl_Position, gl_PointSize, and something else you'll probably never use
//in vec2 tcPos[];
//out vec2 tePos[];
//out gl_out[];

void main()
{
	if(gl_InvocationID == 0)
	{
		gl_TessLevelOuter[0] = 1;
		gl_TessLevelOuter[1] = 64;		//Change this
//		gl_TessLevelOuter[2] = 64;		//Change this
	}

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	teColour[gl_InvocationID] = tcColour[gl_InvocationID];
	
}

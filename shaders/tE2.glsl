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
//layout(triangles) in;
layout( triangles, equal_spacing, ccw)  in;
//in vec2 Tri;
//in vec3 Colour1;
//in vec2 tcPos[];
//out vec2 Pos;
in vec3 teColour[];
out vec3 Colour;
//out vec3 Colour;
uniform int mode;
uniform int scene;
uniform int isTTF;
//uniform int fill;
uniform float xoffset;
uniform float yoffset;
uniform float zoom;
uniform float velocity;

void main()
{
	vec2 fillpos;
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	float w = gl_TessCoord.z;

	float b0 = 1.0-u;
	float b1 = u;
	vec3 startColour = teColour[1];
	vec3 endColour = teColour[1];
	vec2 startp = (gl_in[0].gl_Position.xy);
	vec2 endp = (gl_in[2].gl_Position.xy);
	vec2 p0 = (gl_in[0].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
	vec2 p1 = (gl_in[1].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
	vec2 p2 = (gl_in[2].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
	vec2 p3 = (b0*b0*p0+2*b1*b0*p1+b1*b1*p2);
	fillpos = p0 + u*(p3-p0) + v*(p2-p0)+vec2(-velocity,0);// + u*(p2-p0);

	gl_Position = vec4(fillpos, 0, 1);	
	Colour = startColour;

}



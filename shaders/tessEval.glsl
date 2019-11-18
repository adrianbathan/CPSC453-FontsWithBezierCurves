// ==========================================================================
// Tesseval program for barebones GLFW boilerplate
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
layout(isolines) in;
//layout( triangles, equal_spacing, ccw)  in;
//layout(triangles) in;
//layout(triangles) out;
//layout(triangles) in;
//layout(triangle_strip) out;


in vec3 teColour[];
//in vec2 tePos[];
//in gl_in[];
//out vec2 TriPos;
//out vec3 Colour1;
out vec3 Colour;

//out vec2 Tri;


//const float pi = 3.141592653589793238462643383279502884197169;
#define PI 3.141592653589793238462643383279502884197169
//uniform int degree;
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
	float u = gl_TessCoord.x;

	float b0 = 1.0-u;
	float b1 = u;
	vec3 startColour = teColour[0];
	vec3 endColour = teColour[1];
	vec2 startp = (gl_in[0].gl_Position.xy);;
	vec2 endp = (gl_in[2].gl_Position.xy);;
	vec2 position, pos1;
	if (scene == 0) {
		if (mode == 0) {
			vec2 p0 = gl_in[0].gl_Position.xy/2.5;
			vec2 p1 = gl_in[1].gl_Position.xy/2.5;
			vec2 p2 = gl_in[2].gl_Position.xy/2.5;
			position = b0*b0*p0+2*b1*b0*p1+b1*b1*p2;
		}
		else {
			vec2 p0 = (gl_in[0].gl_Position.xy-5)/5.5+vec2(0.1,0.4);
			vec2 p1 = (gl_in[1].gl_Position.xy-5)/5.5+vec2(0.1,0.4);
			vec2 p2 = (gl_in[2].gl_Position.xy-5)/5.5+vec2(0.1,0.4);
			vec2 p3 = (gl_in[3].gl_Position.xy-5)/5.5+vec2(0.1,0.4);
			position = b0*b0*b0*p0+3*b1*b0*b0*p1+3*b1*b1*b0*p2+b1*b1*b1*p3;
		}
	}
	else if (scene == 1) {
		if (isTTF == 1) {
			vec2 p0 = (gl_in[0].gl_Position.xy+vec2(xoffset,yoffset)-vec2(zoom/2,0))/2;
			vec2 p1 = (gl_in[1].gl_Position.xy+vec2(xoffset,yoffset)-vec2(zoom/2,0))/2;
			vec2 p2 = (gl_in[2].gl_Position.xy+vec2(xoffset,yoffset)-vec2(zoom/2,0))/2;
			position = (b0*b0*p0+2*b1*b0*p1+b1*b1*p2);
		}
		else {
			vec2 p0 = (gl_in[0].gl_Position.xy+vec2(xoffset,yoffset)-vec2(zoom/2,0))/2;
			vec2 p1 = (gl_in[1].gl_Position.xy+vec2(xoffset,yoffset)-vec2(zoom/2,0))/2;
			vec2 p2 = (gl_in[2].gl_Position.xy+vec2(xoffset,yoffset)-vec2(zoom/2,0))/2;
			vec2 p3 = (gl_in[3].gl_Position.xy+vec2(xoffset,yoffset)-vec2(zoom/2,0))/2;
			position = (b0*b0*b0*p0+3*b1*b0*b0*p1+3*b1*b1*b0*p2+b1*b1*b1*p3);
		}
	}
	else if (scene == 2) {
		if (isTTF == 1) {
			vec2 p0 = (gl_in[0].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
			vec2 p1 = (gl_in[1].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
			vec2 p2 = (gl_in[2].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
			position = (b0*b0*p0+2*b1*b0*p1+b1*b1*p2)+vec2(-velocity,0);
		}
		else {
			vec2 p0 = (gl_in[0].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
			vec2 p1 = (gl_in[1].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
			vec2 p2 = (gl_in[2].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
			vec2 p3 = (gl_in[3].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
			position = (b0*b0*b0*p0+3*b1*b0*b0*p1+3*b1*b1*b0*p2+b1*b1*b1*p3)+vec2(-velocity,0);
		}
	}
	else if (scene == 3) {
		if (isTTF == 1) {
			vec2 p0 = (gl_in[0].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
			vec2 p1 = (gl_in[1].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
			vec2 p2 = (gl_in[2].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
			position = (b0*b0*p0+2*b1*b0*p1+b1*b1*p2)+vec2(-velocity,0);
		}
		else {
			vec2 p0 = (gl_in[0].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
			vec2 p1 = (gl_in[1].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
			vec2 p2 = (gl_in[2].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
			vec2 p3 = (gl_in[3].gl_Position.xy+vec2(xoffset,yoffset))/2-vec2(1,0);
			position = (b0*b0*b0*p0+3*b1*b0*b0*p1+3*b1*b1*b0*p2+b1*b1*b1*p3)+vec2(-velocity,0);
		}
	}
	else {
		if (isTTF == 1) {
			vec2 p0 = (gl_in[0].gl_Position.xy);
			vec2 p1 = (gl_in[1].gl_Position.xy);
			vec2 p2 = (gl_in[2].gl_Position.xy);
			position = (b0*b0*p0+2*b1*b0*p1+b1*b1*p2);
		}
		else {
			vec2 p0 = (gl_in[0].gl_Position.xy);
			vec2 p1 = (gl_in[1].gl_Position.xy);
			vec2 p2 = (gl_in[2].gl_Position.xy);
			vec2 p3 = (gl_in[3].gl_Position.xy);
			position = (b0*b0*b0*p0+3*b1*b0*b0*p1+3*b1*b1*b0*p2+b1*b1*b1*p3);
		}
	}
			
	gl_Position = vec4(position, 0, 1);	

//	TriPos = b0*startp-b1*endp;
	Colour = startColour;// b0*startColour; + b1*startColour;
//	Tri = gl_TessCoord.x*startp+gl_TessCoord.y*endp+gl_TessCoord.z*position;
//	Colour0 = endColour;

}

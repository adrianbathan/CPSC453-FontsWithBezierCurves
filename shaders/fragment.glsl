// ==========================================================================
// Vertex program for barebones GLFW boilerplate
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

// interpolated colour received from vertex stage
in vec3 Colour;
//in vec3 TPos;
//in vec2 Pos;
in vec2 UV;

out vec4 FragmentColour;

uniform int scene;
//uniform int ;

uniform sampler2D ourTexture;
uniform int bg;
// first output is mapped to the framebuffer's colour index by default

void main(void)
{
    // write colour output without modification
		if (scene == 4 && bg == 1)
			FragmentColour = texture(ourTexture, UV);//*0.5f;
		else
			FragmentColour = vec4(Colour, 0);

}

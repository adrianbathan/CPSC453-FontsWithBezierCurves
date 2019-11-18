#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing textures

struct MyTexture
{
	GLuint textureID;
	GLuint target;
	int width;
	int height;
	GLuint fboID;

	// initialize object names to zero (OpenGL reserved value)
	MyTexture();
};

bool InitializeTexture(MyTexture* texture, const char* filename, GLuint target = GL_TEXTURE_2D);
bool InitializeFBO(MyTexture* texture, GLuint target = GL_TEXTURE_2D);
void setFBOdimension(MyTexture* fbo, MyTexture* te);
// deallocate texture-related objects
void DestroyTexture(MyTexture *texture);

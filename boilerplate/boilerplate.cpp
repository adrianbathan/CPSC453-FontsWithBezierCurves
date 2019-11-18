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


#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <vector>
#include <tuple>
#include <map>
#include <list>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "texture.h"
#include "geometry.h"
#include "GlyphExtractor.h"

using namespace std;
using namespace glm;
// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

void QueryGLVersion();
bool CheckGLErrors();

string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint tcsShader, GLuint tesShader);//, GLuint tc2Shader, GLuint te2Shader );
//GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

// --------------------------------------------------------------------------
// Functions to set up OpenGL shader programs for rendering

// load, compile, and link shaders, returning true if successful
GLuint InitializeShaders()
{
	// load shader source from files
	string vertexSource = LoadSource("shaders/vertex.glsl");
	string fragmentSource = LoadSource("shaders/fragment.glsl");
	string tcsSource = LoadSource("shaders/tessControl.glsl");
	string tesSource = LoadSource("shaders/tessEval.glsl");
	if (vertexSource.empty() || fragmentSource.empty()) return false;

	// compile shader source into shader objects
	GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	GLuint tcs = CompileShader(GL_TESS_CONTROL_SHADER, tcsSource);
	GLuint tes = CompileShader(GL_TESS_EVALUATION_SHADER, tesSource);

	// link shader program
	GLuint program = LinkProgram(vertex, fragment, tcs, tes);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(tcs);
	glDeleteShader(tes);

	// check for OpenGL errors and return false if error occurred
	return program;
}
GLuint InitializeFillShaders()
{
	// load shader source from files
	string vertexSource = LoadSource("shaders/vertex.glsl");
	string fragmentSource = LoadSource("shaders/fragment.glsl");
	string tcsSource = LoadSource("shaders/tC2.glsl");
	string tesSource = LoadSource("shaders/tE2.glsl");
	if (vertexSource.empty() || fragmentSource.empty()) return false;

	// compile shader source into shader objects
	GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	GLuint tcs = CompileShader(GL_TESS_CONTROL_SHADER, tcsSource);
	GLuint tes = CompileShader(GL_TESS_EVALUATION_SHADER, tesSource);

	// link shader program
	GLuint program = LinkProgram(vertex, fragment, tcs, tes);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(tcs);
	glDeleteShader(tes);

	// check for OpenGL errors and return false if error occurred
	return program;
}
GLuint InitializeTriShaders()
{
	// load shader source from files
	string vertexSource = LoadSource("shaders/vertex2.glsl");
	string fragmentSource = LoadSource("shaders/fragment.glsl");
	if (vertexSource.empty() || fragmentSource.empty()) return false;

	// compile shader source into shader objects
	GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	// link shader program
	GLuint program = LinkProgram(vertex, fragment, 0, 0);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
\
	// check for OpenGL errors and return false if error occurred
	return program;
}

// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer

int scene = 0;
int mode = 0;
int fmode = 3;
int speed = 0, oldspeed = 0;
bool lclickdown = false, pause = false;
double dx = 0, dy = 0, xclick = 0, yclick = 0;
GlyphExtractor myGE[7];
char* fonts[7] = { "Lora-Regular.ttf", "SourceSansPro-Regular.otf", "Mutlu__Ornamental.ttf",//rothenbg.ttf",
	"AlexBrush-Regular.ttf", "Inconsolata.otf", "Amadeus.ttf", "NewRocker-Regular.otf" };//"clutchee.otf" };//"Yellowtail-Regular.otf" };//GreatVibes-Regular.otf" };
//bool ttf[6] = { true, false, true, true, false, false };
int ttf[7] = { 1, 0, 1, 1, 0, 1, 0 };
float xgap = 0, ygap = 0;
//map<char,int> alphaCode;
float letterGaps[7][95];
float nameScale = 1;
float movec = 0.0f;

vector<vec2> customSplines;
vec2 prevPoint;
Geometry mySpline, splineLines;
int selected = -1;
int startContour = 0;


void RenderTexture(Geometry *geometry, GLuint program, MyTexture *tex)
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, tex->textureID);
	
	glUseProgram(program);

	glUniform1i(glGetUniformLocation(program, "scene"), scene);
	glUniform1i(glGetUniformLocation(program, "bg"), 1);
	
	glBindVertexArray(geometry->vertexArray);
	glDrawArrays(GL_TRIANGLES, 0, geometry->elementCount);
	// reset state to default (no shader or geometry bound)
	glBindVertexArray(0);
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	
	// check for an report any OpenGL errors
	CheckGLErrors();
}

//void RenderScene(Geometry *geometry,Geometry *fbogeo, MyTexture *tex, GLuint program)
void RenderScene(Geometry *geometry, GLuint program, bool clr, int tri)
{
	// clear screen to a dark grey colour
	if (clr) {
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	
	glUseProgram(program);
//	mat4 per = perspective(radians(30.0f),1.0f,.1f,1.0f);
	glUniform1i(glGetUniformLocation(program, "bg"), 0);
	glUniform1i(glGetUniformLocation(program, "mode"), mode);
	glUniform1i(glGetUniformLocation(program, "scene"), scene);
//	glUniformMatrix4fv(glGetUniformLocation(program, "perspective"), 1, GL_FALSE, &per[0][0]);
	if (tri==2 && scene == 4)
		glUniform1i(glGetUniformLocation(program, "pen"), 0);
	else
		glUniform1i(glGetUniformLocation(program, "pen"), 1);
	if (tri==0 && scene == 4)
		glUniform1i(glGetUniformLocation(program, "isTTF"), 0);
	else
		glUniform1i(glGetUniformLocation(program, "isTTF"), ttf[mode]);
	glUniform1f(glGetUniformLocation(program, "xoffset"), xgap);
	glUniform1f(glGetUniformLocation(program, "yoffset"), ygap);
//	glUniform1f(glGetUniformLocation(program, "zoom"), nameScale[mode]);
	glUniform1f(glGetUniformLocation(program, "zoom"), nameScale);
	glUniform1f(glGetUniformLocation(program, "velocity"), movec);
//	if (scene == 0)
	glUniform1i(glGetUniformLocation(program, "level"), speed);
//	glUniform1i(glGetUniformLocation(program, "fill"), 0);
//	glUniform1i(glGetUniformLocation(program, "skip"), scene);

	glBindVertexArray(geometry->vertexArray);
	if (tri == 1)
		glDrawArrays(GL_LINES, 0, geometry->elementCount);
	else if (tri == 2)
		glDrawArrays(GL_POINTS, 0, geometry->elementCount);		
	else if (tri == 3)
		glDrawArrays(GL_TRIANGLES, 0, geometry->elementCount);		
	else if (tri == 4)
		glDrawArrays(GL_LINE_STRIP, 0, geometry->elementCount);			
	else
		glDrawArrays(GL_PATCHES, 0, geometry->elementCount);

	// reset state to default (no shader or geometry bound)
	glBindVertexArray(0);
	glUseProgram(0);

	// check for an report any OpenGL errors
	CheckGLErrors();
}
// --------------------------------------------------------------------------
// GLFW callback functions


// reports GLFW errors

void ErrorCallback(int error, const char* description)
{
	cout << "GLFW ERROR " << error << ":" << endl;
	cout << description << endl;
}


// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_S && action == GLFW_PRESS && scene == 4) {
		ofstream saveData;
		saveData.open("save.txt");
		saveData.clear();
		for (vec2 vdata : customSplines) {
			saveData << vdata.x << " " << vdata.y;// >> endl;
		}
		cout << "SPLINE SAVED" << endl;
		saveData.close();
		
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS && scene == 4) {
		ifstream saveData;
		float loadx, loady;
		saveData.open("save.txt", ifstream::in);
		customSplines.clear();
		while (saveData>> loadx >>loady) {
			customSplines.emplace_back(vec2(loadx,loady));
		}
		int nsplines = customSplines.size();
		vec2 splinevertices[nsplines];
		vec2 splinetextures[nsplines];
		vec3 splinecolors[nsplines];
		for (int i=0; i<nsplines; i++) {
			splinevertices[i] = customSplines[i];
			splinecolors[i] = vec3(0.0f, 1.0f, 0.0f);
//			cout << customSplines[i].x << "," << customSplines[i].y << endl;
		}
		LoadGeometry(&splineLines, splinevertices, splinecolors, splinetextures, nsplines);
		LoadGeometry(&mySpline, splinevertices, splinecolors, splinetextures, nsplines);
		saveData.close();
		startContour = customSplines.size();
		cout << "SPLINE LOADED" << endl;
		
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS && scene == 4) {
		vec2 splinevertices[0];
		vec2 splinetextures[0];
		vec3 splinecolors[0];
		customSplines.clear();
		LoadGeometry(&splineLines, splinevertices, splinecolors, splinetextures, 0);
		LoadGeometry(&mySpline, splinevertices, splinecolors, splinetextures, 0);
		cout << "SPLINE CLEARED" << endl;
	}
	if (key == GLFW_KEY_1 && action == GLFW_PRESS && scene != 0) {
		scene = 0;
		mode = 0;
		speed = 0;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS && scene != 1) {
		scene = 1;
		mode = 0;
		speed = 0;
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS && scene != 2) {
		scene = 2;
		mode = fmode;
		speed = 0;
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS && scene != 3) {
		scene = 3;
		mode = 6;
		speed = 0;
	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS && scene != 4) {
		scene = 4;
		mode = 0;
		speed = 0;
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		if (scene == 0) {
			if (mode == 1) 
				mode = 0;
			else
				mode++;
		}
		if (scene == 1) {
			if (mode == 2) 
				mode = 0;
			else
				mode++;
		}
		if (scene == 2) {
			if (fmode == 5) 
				fmode = 3;
			else
				fmode++;
			mode = fmode;
		}
		if (scene == 4) {
			if (mode == 1) 
				mode = 0;
			else
				mode = 1;
		}
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		if (scene == 0) {
			if (mode == 0) 
				mode = 1;
			else
				mode--;
		}
		if (scene == 1) {
			if (mode == 0) 
				mode = 2;
			else
				mode--;
		}
		if (scene == 2) {
			if (fmode == 3) 
				fmode = 5;
			else
				fmode--;
			mode = fmode;
		}
		if (scene == 4) {
			if (mode == 0) 
				mode = 1;
			else
				mode = 0;
		}
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		if ((scene == 2  || scene == 3) && (!pause)) {
			if (speed == 10) 
				speed = 10;
			else
				speed++;
		}
		if (scene == 0) {
			if (speed == 2)
				speed = 0;
			else
				speed++;
		}
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		if ((scene == 2  || scene == 3) && (!pause)) {
			if (speed == -10) 
				speed = -10;
			else
				speed--;
		}
		if (scene == 0) {
			if (speed == 0)
				speed = 2;
			else
				speed--;
		}
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		if (scene == 2  || scene == 3) {
			if (!pause) {
				pause = true;
				cout << "SCROLL PAUSED" << endl;
				oldspeed = speed;
				speed = 0;
			}
			else {
				pause = false;
				cout << "SCROLL RESUMING" << endl;
				speed = oldspeed;
			}
		}
	}
}

void updateGEO() {
	int nsplines = customSplines.size();
	vec2 splinevertices[nsplines];
	vec2 splinetextures[nsplines];
	vec3 splinecolors[nsplines];
	for (int i=0; i<nsplines; i++) {
		splinevertices[i] = customSplines[i];
		splinecolors[i] = vec3(0.0f, 1.0f, 0.0f);
	}
	LoadGeometry(&splineLines, splinevertices, splinecolors, splinetextures, nsplines);
	LoadGeometry(&mySpline, splinevertices, splinecolors, splinetextures, nsplines);
}


 // mouse controlls
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	cout.precision(8);
	
	double x_raw, y_raw, x_ogl, y_ogl;//, dx0,dy0;
	if (lclickdown && selected != -1) {	
		glfwGetCursorPos(window, &x_raw, &y_raw);
		x_ogl = (-512+x_raw)/512;
		y_ogl = (512-y_raw)/512;
		dx = x_ogl-xclick;
		dy = y_ogl-yclick;
		customSplines[selected] = customSplines[selected] + vec2(dx,dy);
		updateGEO();
		dx = 0; 
		dy = 0;
		xclick = x_ogl;
		yclick = y_ogl;
		prevPoint = customSplines.back();
	}	
}

int onPoint() {
	for (int k=0; k<customSplines.size(); k++) {
		if (abs(distance(customSplines[k], vec2((float)xclick, (float)yclick)))<=0.025)
			return k;
	}
	return  -1;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double x, y;
	if (scene == 4) {
		glfwGetCursorPos(window, &x, &y);
		xclick = (-512+x)/512;
		yclick = (512-y)/512;
		
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{	
			lclickdown = true;
			dx = 0;
			dy = 0;
			if (onPoint() == -1) {
				if ((customSplines.size()%4==0) && customSplines.size()!=startContour)//0)
			{		customSplines.push_back(prevPoint);
				customSplines.push_back(prevPoint*2.0f/3.0f+vec2((float)xclick, (float)yclick)/3.0f);
				customSplines.push_back(prevPoint/3.0f+vec2((float)xclick, (float)yclick)*2.0f/3.0f);}
				if ((customSplines.size()%4 == 1) && customSplines.size()==startContour+1) {
					customSplines.push_back(prevPoint*2.0f/3.0f+vec2((float)xclick, (float)yclick)/3.0f);
					customSplines.push_back(prevPoint/3.0f+vec2((float)xclick, (float)yclick)*2.0f/3.0f);}
				customSplines.push_back(vec2((float)xclick, (float)yclick));
				prevPoint = customSplines.back();
			}
			else if (onPoint() == startContour && customSplines.size()>1) {
				if (customSplines.size()%4!=0)
					for (int i=0; i<customSplines.size()%4;i++)
						customSplines.push_back(customSplines[startContour]);
				else {
				customSplines.push_back(prevPoint);
				customSplines.push_back(prevPoint*2.0f/3.0f+vec2((float)xclick, (float)yclick)/3.0f);
				customSplines.push_back(prevPoint/3.0f+vec2((float)xclick, (float)yclick)*2.0f/3.0f);
				customSplines.push_back(customSplines[startContour]);}
				startContour = customSplines.size();
			}
			else {
				selected = onPoint();
			}
			updateGEO();
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		{
			lclickdown = false;
			selected = -1;
			dx=0;
			dy=0;
//			cout << "----------------------" << endl;			
//			for (int i=0; i<customSplines.size(); i++) {
//				cout << customSplines[i].x << "," << customSplines[i].y << " : " << startContour  << endl;
//		}
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		{
			if (onPoint() != -1) 
				customSplines.erase(customSplines.begin()+onPoint());
			updateGEO();

		}
		
	}
}



// ==========================================================================
// PROGRAM ENTRY POINT


vector<tuple<GLfloat,GLfloat>> bezVer(int f) {
	vector<tuple<GLfloat,GLfloat>> points;
	int n;
	if (scene == 0) {
		if (f == 0) {
			points.emplace_back( 1.0f, 1.0f );
			points.emplace_back( 2.0f, -1.0f );
			points.emplace_back( 0.0f, -1.0f );
			points.emplace_back( 0.0f, -1.0f );
			
			points.emplace_back( 0.0f, -1.0f );
			points.emplace_back( -2.0f, -1.0f );
			points.emplace_back( -1.0f, 1.0f );
			points.emplace_back( -1.0f, 1.0f );
			
			points.emplace_back( -1.0f, 1.0f );
			points.emplace_back( 0.0f, 1.0f );
			points.emplace_back( 1.0f, 1.0f );
			points.emplace_back( 1.0f, 1.0f );
			
			points.emplace_back( 1.2f, .5f );
			points.emplace_back( 2.5f, 1.0f );
			points.emplace_back( 1.3f, -.4f );
			points.emplace_back( 1.3f, -.4f );
			n = 16;
		}
		else {
			points.emplace_back( 1.0f, 1.0f );
			points.emplace_back( 4.0f, 0.0f );
			points.emplace_back( 6.0f, 2.0f );
			points.emplace_back( 9.0f, 1.0f );
			
			points.emplace_back( 8.0f, 2.0f );
			points.emplace_back( 0.0f, 8.0f );
			points.emplace_back( .0f, -2.0f );
			points.emplace_back( 8.0f, 4.0f );
			
			points.emplace_back( 5.0f, 3.0f );
			points.emplace_back( 3.0f, 2.0f );
			points.emplace_back( 3.0f, 3.0f );
			points.emplace_back( 5.0f, 2.0f );
			
			points.emplace_back( 3.0f, 2.2f );
			points.emplace_back( 3.5f, 2.7f );
			points.emplace_back( 3.5f, 3.3f );
			points.emplace_back( 3.0f, 3.8f );
			
			points.emplace_back( 2.8f, 3.5f );
			points.emplace_back( 2.4f, 3.8f );
			points.emplace_back( 2.4f, 3.2f );
			points.emplace_back( 2.8f, 3.5f );
			n = 20;
		}
	}
	return points;

}
vector<tuple<GLfloat,GLfloat,GLfloat>> bezCol(int f) {
	vector<tuple<GLfloat,GLfloat,GLfloat>> colors;
	int n;
	if (scene == 0) {
		if (f == 0) {
			for (int i=0; i<16; i++)
				colors.emplace_back( 1.0f, 0.0f, 0.0f);
			n = 16;
		}
		else {
			for (int i=0; i<20; i++)
				colors.emplace_back( 1.0f, 0.0f, 0.0f);
			n = 20;
		}
	}
	return colors;
}

vector<tuple<GLfloat,GLfloat>> bezLinesVer(int f, vector<tuple<GLfloat,GLfloat>> v, int m) {
vector<tuple<GLfloat,GLfloat>> points;
	int n;
	if (scene == 0) {
		if (f == 0) {
			points.emplace_back( 1.0f, 1.0f );
			points.emplace_back( 2.0f, -1.0f );
			points.emplace_back( 2.0f, -1.0f );
			points.emplace_back( 0.0f, -1.0f );
			
			points.emplace_back( 0.0f, -1.0f );
			points.emplace_back( -2.0f, -1.0f );
			points.emplace_back( -2.0f, -1.0f );
			points.emplace_back( -1.0f, 1.0f );
			
			points.emplace_back( -1.0f, 1.0f );
			points.emplace_back( 0.0f, 1.0f );
			points.emplace_back( 0.0f, 1.0f );
			points.emplace_back( 1.0f, 1.0f );
			
			points.emplace_back( 1.2f, .5f );
			points.emplace_back( 2.5f, 1.0f );
			points.emplace_back( 2.5f, 1.0f );
			points.emplace_back( 1.3f, -.4f );
			n = 16;
		}
		else {
			points.emplace_back( 1.0f, 1.0f );
			points.emplace_back( 4.0f, 0.0f );
			points.emplace_back( 4.0f, 0.0f );
			points.emplace_back( 6.0f, 2.0f );
			points.emplace_back( 6.0f, 2.0f );
			points.emplace_back( 9.0f, 1.0f );
			
			points.emplace_back( 8.0f, 2.0f );
			points.emplace_back( 0.0f, 8.0f );
			points.emplace_back( 0.0f, 8.0f );
			points.emplace_back( .0f, -2.0f );
			points.emplace_back( .0f, -2.0f );
			points.emplace_back( 8.0f, 4.0f );
			
			points.emplace_back( 5.0f, 3.0f );
			points.emplace_back( 3.0f, 2.0f );
			points.emplace_back( 3.0f, 2.0f );
			points.emplace_back( 3.0f, 3.0f );
			points.emplace_back( 3.0f, 3.0f );
			points.emplace_back( 5.0f, 2.0f );
			
			points.emplace_back( 3.0f, 2.2f );
			points.emplace_back( 3.5f, 2.7f );
			points.emplace_back( 3.5f, 2.7f );
			points.emplace_back( 3.5f, 3.3f );
			points.emplace_back( 3.5f, 3.3f );
			points.emplace_back( 3.0f, 3.8f );
			
			points.emplace_back( 2.8f, 3.5f );
			points.emplace_back( 2.4f, 3.8f );
			points.emplace_back( 2.4f, 3.8f );
			points.emplace_back( 2.4f, 3.2f );
			points.emplace_back( 2.4f, 3.2f );
			points.emplace_back( 2.8f, 3.5f );
			n = 30;
		}
	}
	return points;

}

vector<tuple<GLfloat,GLfloat,GLfloat>> bezLinesCol(int f, vector<tuple<GLfloat,GLfloat,GLfloat>> c, int m) {
	vector<tuple<GLfloat,GLfloat,GLfloat>> colors;
	int n = c.size();
	for (int i=0; i<n; i++) {
		if (f == 0) {
			if (i%4 == 1) {
				if (m == 0) {
					for (int j=0; j<4; j++)
						colors.emplace_back( 0.0f, 1.0f, 0.0f);
				}
				else {
					for (int j=0; j<16; j++)
						colors.emplace_back( 1.0f, 1.0f, 1.0f);
				}		
			}
			else if (i%4 != 3) {
				if (m == 0) {
					for (int j=0; j<2; j++)
						colors.emplace_back( 0.0f, 1.0f, 0.0f);
				}
				else {
					for (int j=0; j<16; j++)
						colors.emplace_back( 1.0f, .67f, 1.0f);
				}
			}
		}
		else {
			if (i%4 == 1 || i%4 == 2) {
				if (m == 0) {
					for (int j=0; j<4; j++)
						colors.emplace_back( 0.0f, 1.0f, 0.0f);
				}
				else {
					for (int j=0; j<16; j++)
						colors.emplace_back( 1.0f, 1.0f, 1.0f);
				}
			}
			else {
				if (m == 0) {
					for (int j=0; j<2; j++)
						colors.emplace_back( 0.0f, 1.0f, 0.0f);
				}
				else {
					for (int j=0; j<16; j++)
						colors.emplace_back( 1.0f, .67f, 1.0f);
				}
			}
		}
	}
	return colors;
}

vector<tuple<GLfloat,GLfloat>> GlyphToVer(int f, int letter) {
	vector<tuple<GLfloat,GLfloat>> points;
	MyGlyph g;
	int isttf = ttf[f];
	g = myGE[f].ExtractGlyph(letter);
	for (MyContour t : g.contours) {
		for (MySegment s : t) {
			if (isttf) {
				if (s.degree == 0) {
					for (int k=0;k<4;k++)
						points.emplace_back(s.x[0],s.y[0]);
				}
				else if (s.degree == 1) {
					for (int k=0;k<2;k++)
						points.emplace_back(s.x[0],s.y[0]);
					for (int k=0;k<2;k++)
						points.emplace_back(s.x[1],s.y[1]);
				}
				else {
					points.emplace_back(s.x[0],s.y[0]);
					points.emplace_back((s.x[1]),s.y[1]);
					points.emplace_back(s.x[2],s.y[2]);
					points.emplace_back(s.x[2],s.y[2]);
				}
			}
			else {
				if (s.degree == 0) {
					for (int k=0;k<4;k++)
						points.emplace_back(s.x[0],s.y[0]);
				}
				else if (s.degree == 1) {
					for (int k=0;k<2;k++)
						points.emplace_back(s.x[0],s.y[0]);
					for (int k=0;k<2;k++)
						points.emplace_back(s.x[1],s.y[1]);
				}
				else if (s.degree == 2) {
					points.emplace_back(s.x[0],s.y[0]);
					points.emplace_back(s.x[1],(s.y[1]*2/3));
					points.emplace_back((s.x[1]*2/3),s.y[1]);
					points.emplace_back(s.x[2],s.y[2]);
				}
				else {
					for (int k=0;k<4;k++)
						points.emplace_back(s.x[k],s.y[k]);
				}
			}
		}
	}
	if (letter == 127 && f==6)
		letterGaps[f][letter-33] = g.advance/2;
	else
		letterGaps[f][letter-33] = g.advance;
	return points;
}

vector<tuple<GLfloat,GLfloat,GLfloat>> GlyphToCol(int f, int letter) {
	vector<tuple<GLfloat,GLfloat,GLfloat>> colors;
	MyGlyph g;
	int n=0;
	g = myGE[f].ExtractGlyph(letter);
	for (MyContour t : g.contours) 
			n = n+4*t.size();
	for (int i=0; i<n; i++)
		colors.emplace_back( 1.0f, .67f, 1.0f);
	return colors;
}

vector<tuple<GLfloat,GLfloat>> FilledGlyphVer(int letter) {
	vector<tuple<GLfloat,GLfloat>> points;
	vector<vector<tuple<GLfloat,GLfloat>>> fullLetter;
	
	tuple<GLfloat,GLfloat> p0, p1, p2, p1a, p2a, p2b, p3, p4;
	float px=0, py=0;
	MyGlyph g;
	g = myGE[6].ExtractGlyph(letter);
	for (MyContour t : g.contours) {
		vector<tuple<GLfloat,GLfloat>> loops;
		for (int i=0; i<t.size(); i++) {
			if (t[i].degree == 1) {
				p0=make_tuple(t[i].x[0],t[i].y[0]);
				p1=make_tuple((t[i].x[0]+t[i].x[1])/2,(t[i].y[0]+t[i].y[1])/2);
				p2=make_tuple(t[i].x[1],t[i].y[1]);
				
				points.emplace_back(make_tuple(0,0));
				points.emplace_back(p0);
				points.emplace_back(p1);
				points.emplace_back(make_tuple(0,0));
				points.emplace_back(p1);
				points.emplace_back(p2);
			}
			else if (t[i].degree == 2) {
				p0=make_tuple(t[i].x[0],t[i].y[0]);
				p2=make_tuple(t[i].x[2],t[i].y[2]);
				px = 0.25f*t[i].x[0]+0.5f*t[i].y[1]+0.25f*t[i].x[2];
				py = 0.25f*t[i].y[0]+0.5f*t[i].y[1]+0.25f*t[i].y[2];
				p1=make_tuple(px,py);
			}
			else {
				p0=make_tuple(t[i].x[0],t[i].y[0]);
				p1a=make_tuple(t[i].x[1],t[i].y[1]);
				p2a=make_tuple(t[i].x[2],t[i].y[2]);
				p4=make_tuple(t[i].x[3],t[i].y[3]);
				px = 0.125f*get<0>(p0)+3*0.125f*get<0>(p1a)+3*0.125f*get<0>(p2a)+0.125f*get<0>(p4);
				py = 0.125f*get<1>(p0)+3*0.125f*get<1>(p1a)+3*0.125f*get<1>(p2a)+0.125f*get<1>(p4);
//				p2b = make_tuple((4*px-get<0>(p0)-get<0>(p4))/2, (4*py-get<1>(p0)-get<1>(p4))/2);
				p2 = make_tuple(px, py);
				px = pow(0.75f,3)*get<0>(p0)+3*pow(0.75f,2)*0.25f*get<0>(p1a)+3*pow(0.25f,2)*0.75f*get<0>(p2a)+pow(0.25f,3)*get<0>(p4);
				py = pow(0.75f,3)*get<1>(p0)+3*pow(0.75f,2)*0.25f*get<1>(p1a)+3*pow(0.25f,2)*0.75f*get<1>(p2a)+pow(0.25f,3)*get<1>(p4);
//				p3a= make_tuple(px, py);
				p1 = make_tuple((4*px-get<0>(p0)-get<0>(p2))/2, (4*py-get<1>(p0)-get<1>(p2))/2);
				px = pow(0.25f,3)*get<0>(p0)+3*pow(0.25f,2)*0.75f*get<0>(p1a)+3*pow(0.75f,2)*0.25f*get<0>(p2a)+pow(0.75f,3)*get<0>(p4);
				py = pow(0.25f,3)*get<1>(p0)+3*pow(0.25f,2)*0.75f*get<1>(p1a)+3*pow(0.75f,2)*0.25f*get<1>(p2a)+pow(0.75f,3)*get<1>(p4);
//				p4a= make_tuple(px, py);
				p3 = make_tuple((4*px-get<0>(p2)-get<0>(p4))/2, (4*py-get<1>(p2)-get<1>(p4))/2);

				points.emplace_back(make_tuple(0,0));
				points.emplace_back(p0);
				points.emplace_back(p1);
				points.emplace_back(make_tuple(0,0));
				points.emplace_back(p1);				
				points.emplace_back(p2);
				points.emplace_back(make_tuple(0,0));
				points.emplace_back(p2);				
				points.emplace_back(p3);
				points.emplace_back(make_tuple(0,0));
				points.emplace_back(p3);
				points.emplace_back(p4);

			}
		}
	}
	return points;
}


int main(int argc, char *argv[])
{
	// initialize the GLFW windowing system
	if (!glfwInit()) {
		cout << "ERROR: GLFW failed to initialize, TERMINATING" << endl;
		return -1;
	}
	glfwSetErrorCallback(ErrorCallback);

	// attempt to create a window with an OpenGL 4.1 core profile context
	GLFWwindow *window = 0;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	int width = 1024, height = 1024;
	window = glfwCreateWindow(width, height, "CPSC 453 OpenGL Boilerplate", 0, 0);
	if (!window) {
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return -1;
	}

	// set keyboard callback function and make our context current (active)
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwMakeContextCurrent(window);
	
	glfwSetCursorPos(window, 512,512);

	//Intialize GLAD
	if (!gladLoadGL())
	{
		cout << "GLAD init failed" << endl;
		return -1;
	}

	// query and print out information about our OpenGL environment
	QueryGLVersion();
	char* alpha = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ ";
	char* myname = "AdrianBathan";
	char *myphrase = "The quick brown fox jumps over the lazy dog.";
//	char *myphrase2 = "This is a shitty attempt fill the text for the bonus...";
	char *myphrase2[13] = {"------Bonus Please------", 
		"Here is a quote from 50 shades of grey:  ",
		"--------------------------------",
		"\"Does this mean you're going to make love",
		"to me tonight, Christian?\" Holy shit. Did",
		"I just say that? His mouth drops open ",
		"slightly, but he recovers quickly. \"No, ",
		"Anastasia, it doesn't. Firstly, I don't ",
		"make love. I fuck... hard. Secondly, ",
		"there's a lot more paperwork to do, and ",
		"thirdly, you don't yet know what you're ",
		"in for. You could still run for the hills.",
		"Come, I want to show you my playroom.\"",
		 };
	int alphaSize = strlen(alpha);
	int alphas[alphaSize];
	for (int i=0; i<alphaSize; i++)
		alphas[i] = i+33;
	int nameSize = strlen(myname);
	int name[nameSize];
	for (int i=0; i<nameSize; i++) 
		name[i] = alphas[strchr(alpha, myname[i])-alpha];
	int phraseSize = strlen(myphrase);
	int phrase[phraseSize];
	for (int i=0; i<phraseSize; i++) 
		phrase[i] = alphas[strchr(alpha, myphrase[i])-alpha];
	vector<int>phrase2[13];
	for (int j=0; j<13; j++) {
	int phraseSize2 = strlen(myphrase2[j]);
//	phrase2[j][phrasesSize2]
//	int phrase2[phraseSize2];
	for (int i=0; i<strlen(myphrase2[j]); i++) 
//		phrase2[i] = alphas[strchr(alpha, myphrase2[i])-alpha];
		phrase2[j].push_back(alphas[strchr(alpha, myphrase2[j][i])-alpha]);
	}
//	
	Geometry curves[2][3];
	Geometry alphabets[7][95];
	Geometry trialpha[95];
	Geometry background;
	MyTexture handwriting;
	if (!InitializeTexture(&handwriting, "fufufu.jpg", GL_TEXTURE_2D))
			cout << "Program failed to initialize texture" << endl;
	float img_h = (float)handwriting.height/2;
	float img_w = (float)handwriting.width/2;
	float coordx = (handwriting.width - img_w)/img_h;
	float coordy = (handwriting.height - img_h)/img_h;
	vec2 verhw[] = {
		vec2( -coordx, -coordy ),
		vec2( coordx, coordy ),
		vec2( -coordx, coordy ),
		vec2( -coordx, -coordy ),
		vec2( coordx, coordy ),
		vec2( coordx, -coordy )
	};
	vec2 texhw[] = {
		vec2( .0f, .0f ),
		vec2( 1.0f,  1.0f ),
		vec2( .0f, 1.0f ),
		vec2( .0f, .0f ),
		vec2( 1.0f,  1.0f ),
		vec2( 1.0f, .0f )
	};
	vec3 colhw[] = {
		vec3( 0.0f, 0.0f, 1.0f ),
		vec3( 0.0f, 1.0f, 0.0f ),
		vec3( 1.0f, 0.0f, 0.0f ),
		vec3( 0.0f, 0.0f, 1.0f ),
		vec3( 0.0f, 1.0f, 0.0f ),
		vec3( 1.0f, 0.0f, 0.0f )
	};
	InitializeVAO(&background);
	LoadGeometry(&background, verhw, colhw, texhw, 6);
	InitializeVAO(&mySpline);
	InitializeVAO(&splineLines);
//	Geometry filledalpha[95][25];
//	vector<vector<Geometry>> filledalpha[95];

	// call function to load and compile shader programs
	GLuint program = InitializeShaders();
	GLuint fillProgram = InitializeFillShaders();
	GLuint triProgram = InitializeTriShaders();
//	program = InitializeShaders();
	if (program == 0) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}
	if (fillProgram == 0) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}
	if (triProgram == 0) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}
	vector<tuple<GLfloat,GLfloat>> v,v1,v2;// = bezVer();
	vector<tuple<GLfloat,GLfloat,GLfloat>> c,c1,c2;// = bezCol();
	int count;// = v.size();
	int nControl;
	float totalw = 0, oldw = 0;

	for (int i=0; i<7; i++) {
		if (!myGE[i].LoadFontFile(fonts[i]))
			cout << "failed to load texture: " << fonts[i] << endl;
	}

	// three vertex positions and assocated colours of a triangle

	vec2 textures[] = {
		vec2( .0f, .0f ),
		vec2( 1.0f,  1.0f ),
		vec2( .0f, 1.0f ),
		vec2( .0f, .0f ),
		vec2( 1.0f,  1.0f ),
		vec2( 1.0f, .0f )
	};


	// call function to create and fill buffers with geometry data
	for (int i=0; i<2; i++) {
		v = bezVer(i);
		c = bezCol(i);
		count = v.size();
		nControl = count;
		vec2 vertices0[count];
		vec3 colours0[count];
		for (int j=0; j<count; j++) {
			vertices0[j] = vec2(get<0>(v[j]),get<1>(v[j]));
			colours0[j] = vec3(1.0f,0.0f,0.0f);	
			
			
		}
		InitializeVAO(&curves[i][0]);
		LoadGeometry(&curves[i][0], vertices0, colours0, textures, count);
				
		v1 = bezLinesVer(i,v,0);
		count = v1.size();
		vec2 vertices1[count];
		vec3 colours1[count];
		vec3 colours2[count];
		vec2 vertices2[count];
		
		for (int j=0; j<count; j++) {
			vertices1[j] = vec2(get<0>(v1[j]),get<1>(v1[j]));
			colours1[j] = vec3(0.0f,1.0f,0.0f);	
			vertices2[j] = vec2(get<0>(v1[j]),get<1>(v1[j]));
			if (i==0) {
				if (j%4==0 || j%4==3)
					colours2[j] = vec3(1.0f,1.0f,1.0f);	
				else
					colours2[j] = vec3(1.0f,.67f,1.0f);	
			}
			else {
				if (j%6==0 || j%6==5)
					colours2[j] = vec3(1.0f,.67f,1.0f);	
				else
					colours2[j] = vec3(1.0f,1.0f,1.0f);	
			}	
		}
		InitializeVAO(&curves[i][1]);
		LoadGeometry(&curves[i][1], vertices1, colours1, textures, count);
		
		InitializeVAO(&curves[i][2]);
		LoadGeometry(&curves[i][2], vertices2, colours2, textures, count);
	}

	for (int i=0; i<7; i++) {
		for (int j=0; j<95; j++) {
			v = GlyphToVer(i,j+33);
			c = GlyphToCol(i,j+33);
			count = v.size();
			vec2 vertices0[count];
			vec3 colours0[count];
			for (int k=0;k<count;k++) {
				vertices0[k] = vec2(get<0>(v[k]),get<1>(v[k]));
				colours0[k] = vec3(get<0>(c[k]),get<1>(c[k]),get<2>(c[k]));	
			}
			InitializeVAO(&alphabets[i][j]);
			LoadGeometry(&alphabets[i][j], vertices0, colours0, textures, count);
		}
	}
	for (int j=0; j<95; j++) {
		v = FilledGlyphVer(j+33);
		count = v.size();
		vec2 vertices0[count];
		vec3 colours0[count];
		for (int k=0;k<count;k++) {
			vertices0[k] = vec2(get<0>(v[k]),get<1>(v[k]));
			colours0[k] = vec3(1.0f, 1.0f, 1.0f);	
		InitializeVAO(&trialpha[j]);
		LoadGeometry(&trialpha[j], vertices0, colours0, textures, count);
		}
	}

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glPointSize(5);
	while (!glfwWindowShouldClose(window))
	{
		if (scene == 0) {				
			RenderScene(&curves[mode][0], program, true, 0);
			if (speed > 0)
				RenderScene(&curves[mode][1], triProgram, false, 1);
			if (speed > 1) {
				RenderScene(&curves[mode][2],triProgram, false, 2);
			}
			RenderScene(&curves[mode][0], program, false, 0);
			}
		else if (scene == 1) {
			totalw = 0;
			for  (int j=6; j<nameSize; j++) 
				totalw = totalw + letterGaps[mode][name[j]-33];
			nameScale = totalw;
			RenderScene(&alphabets[mode][name[0]-33], program, true, 0);
			for (int k=1; k<nameSize; k++) {
				if (k!=6) 
					xgap = xgap + letterGaps[mode][name[k-1]-33];
				if (name[k]!=127)
					RenderScene(&alphabets[mode][name[k]-33], program, false, 0);
				if (k==5) {
					xgap = 0;
					ygap = -0.8;
				}
			}
			xgap = 0;
			ygap = 0;
			nameScale = 1;
		}
		else if (scene == 2) {
			totalw = 0;
			for  (int j=0; j<phraseSize; j++) 
				totalw = totalw + letterGaps[fmode][phrase[j]-33];
			nameScale = totalw;
			if (oldw != 0) {
				if (oldw != totalw) 
					movec=movec*totalw/oldw;
			}
			if (movec > totalw/2)
				movec = -2;
			else if (movec < -2)
				movec = totalw/2;
			else
				movec = movec + totalw*0.0005f*speed;
			RenderScene(&alphabets[fmode][phrase[0]-33], program, true, 0);
			for (int k=1; k<phraseSize; k++) {
				xgap = xgap + letterGaps[fmode][phrase[k-1]-33];
				if (phrase[k]!=127)
					RenderScene(&alphabets[fmode][phrase[k]-33], program, false, 0);
			}
			oldw = totalw;
			xgap = 0;
			ygap = 0;
		}
		else if (scene == 3) {
//			movec = -1;
			for (int h=0; h<13; h++) {
			totalw = 0;
			nameScale = 0;
//			for  (int j=0; j<phraseSize2; j++) 
			for  (int j=0; j<phrase2[h].size(); j++) 
				totalw = totalw + letterGaps[6][phrase2[h][j]-33];
			nameScale = totalw;
//			if (oldw != 0) {
//				if (oldw != totalw) 
//					movec=movec*totalw/oldw;
//			}
			if (movec > 16)//totalw/2)
				movec = 0;//2;
			else if (movec < 0)//-2)
				movec = 16;//totalw/2;
			else
				movec = movec + /*totalw**/0.0002f*speed;
			glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
			glEnable(GL_BLEND);
			if (h == 0)
				RenderScene(&trialpha[phrase2[0][0]-33], triProgram, true, 3);
			else
				RenderScene(&trialpha[phrase2[h][0]-33], triProgram, false, 3);
			for (int k=1; k<phrase2[h].size(); k++) {
				xgap = xgap + letterGaps[6][phrase2[h][k-1]-33];
				if (phrase2[h][k]!=127) 
					RenderScene(&trialpha[phrase2[h][k]-33], triProgram, false, 3);
			}
			glBlendFunc(GL_ONE, GL_ZERO);
			glDisable(GL_BLEND);
//			oldw = totalw;
			xgap = 0;
			ygap = -h-12;
		}
	}
		else if (scene == 4) {
			RenderScene(&splineLines, program, true, 0);
			if (mode == 0)
				RenderTexture(&background, triProgram, &handwriting);
			RenderScene(&mySpline, triProgram, false, 2);
			RenderScene(&splineLines, program, false, 0);
		}
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// clean up allocated resources before exit
	for (int i=0; i<2; i++) 
		for (int j=0; j<3; j++) 
			DestroyGeometry(&curves[i][j]);
	for (int i=0; i<7; i++) 
		for (int j=0; j<95; j++) 
			DestroyGeometry(&alphabets[i][j]);
	DestroyGeometry(&background);
	DestroyGeometry(&splineLines);
	DestroyGeometry(&mySpline);
	glUseProgram(0);
	glDeleteProgram(program);
	glDeleteProgram(fillProgram);
	glDeleteProgram(triProgram);
	glfwDestroyWindow(window);
	glfwTerminate();

	cout << "Goodbye!" << endl;
	return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
	// query opengl version and renderer information
	string version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	string glslver = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

	cout << "OpenGL [ " << version << " ] "
		<< "with GLSL [ " << glslver << " ] "
		<< "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors()
{
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "OpenGL ERROR:  ";
		switch (flag) {
		case GL_INVALID_ENUM:
			cout << "GL_INVALID_ENUM" << endl; break;
		case GL_INVALID_VALUE:
			cout << "GL_INVALID_VALUE" << endl; break;
		case GL_INVALID_OPERATION:
			cout << "GL_INVALID_OPERATION" << endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
		case GL_OUT_OF_MEMORY:
			cout << "GL_OUT_OF_MEMORY" << endl; break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
	string source;

	ifstream input(filename.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			istreambuf_iterator<char>(),
			back_inserter(source));
		input.close();
	}
	else {
		cout << "ERROR: Could not load shader source from file "
			<< filename << endl;
	}

	return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
	// allocate shader object name
	GLuint shaderObject = glCreateShader(shaderType);

	// try compiling the source as a shader of the given type
	const GLchar *source_ptr = source.c_str();
	glShaderSource(shaderObject, 1, &source_ptr, 0);
	glCompileShader(shaderObject);

	// retrieve compile status
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
		cout << "ERROR compiling shader:" << endl << endl;
		cout << source << endl;
		cout << info << endl;
	}

	return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
//GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint tcsShader, GLuint tesShader)//, GLuint tc2Shader, GLuint te2Shader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (fragmentShader) glAttachShader(programObject, fragmentShader);
	if (tcsShader) glAttachShader(programObject, tcsShader);
	if (tesShader) glAttachShader(programObject, tesShader);
//	if (tc2Shader) glAttachShader(programObject, tc2Shader);
//	if (te2Shader) glAttachShader(programObject, te2Shader);

	// try linking the program with given attachments
	glLinkProgram(programObject);

	// retrieve link status
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}

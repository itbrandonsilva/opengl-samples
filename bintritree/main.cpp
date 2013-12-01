#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include <map>
#include <direct.h>
using namespace std;

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

int w;
int h;

GLuint ShaderId;
GLuint vbID;
int numPoints = 0;
glm::vec3 vertices[100000];

#include "BTT.h"
#include "input.h"

BTT* TreeOne = new BTT();
BTT* TreeTwo = new BTT();

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){
 
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
 
    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }
 
    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }
 
    GLint Result = GL_FALSE;
    int InfoLogLength;
 
    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
 
    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
 
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
 
    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
 
    // Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
 
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage(InfoLogLength+1);
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
 
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
 
    return ProgramID;
}

bool InitializeWindow() {

	int width = WINDOW_WIDTH;
	int height = WINDOW_HEIGHT;

	// Initializes random number seed.
	srand(time(NULL));

	if( !glfwInit() ) {
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return false;
	}

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4); // 4x antialiasing
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL
 
	// Open a window and create its OpenGL context
	if( !glfwOpenWindow( WINDOW_WIDTH, WINDOW_HEIGHT, 0,0,0,0, 32,0, GLFW_WINDOW ) ) {
		fprintf( stderr, "Failed to open GLFW window\n" );
		glfwTerminate();
		return false;
	}
 
	// Initialize GLEW
	glewExperimental = true; // Needed in core profile
	if ( glewInit() != GLEW_OK ) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return false;
	}

	// This error is expected; there are no problems;
	cout << "\nError: " << glGetError();

	// Create and compile our GLSL program from the shaders
	ShaderId = LoadShaders( "shaderVertex.gls", "shaderFragment.gls" );
	glUseProgram(ShaderId);
 
	glfwSetWindowTitle( "BinTriTree" );

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Ensure we can capture the escape key
	glfwEnable( GLFW_STICKY_KEYS );

	// This is required by OpenGL for some reason. The tutorial says it's "not important" and does not explain what this does.
	GLuint vID;
	glGenVertexArrays(1, &vID);
	glBindVertexArray(vID);

	// Set the color used in calls to glClear() to a dark blue
	// glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
	glClearColor((float)250/255, (float)250/255, (float)210/255, 0.0f);

	//glLineWidth(2.5f);
	glLineWidth(0.5f);

	// Disables backface culling; needed for transparency?
	//glDisable(GL_CULL_FACE);
	//glDepthMask(GL_FALSE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//GLuint VertexArrayID;
	//glGenVertexArrays(1, &VertexArrayID);
	//glBindVertexArray(VertexArrayID);

	cout << "\n\nError: " << glGetError();

	char thePath[256];
	_getcwd(thePath, 255);
	cout << "\nWorking Directory: " << thePath << std::endl;

	return true;
}

GLubyte *pixels;
GLint internalFormat, width, height;
int LoadHeightmap() {

	GLuint textureId = 1;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	// Image must not be grayscale; convert it to RGBA if it is.
	glfwLoadTexture2D( "heightmap.tga", GLFW_ALPHA_MAP_BIT );

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width); // get width of GL texture
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height); // get height of GL texture

	// Is there a better way to allocate this memory in C++?
	int bytes = width * height * 4;
	pixels = (GLubyte *)malloc(width*height*4);

	// Store pixel data in memory for LOD.
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {

			// Heightmaps don't render the same in every editor.
			int addr = ((y*width) + x)*4;
			pixels[addr]    = (int)pixels[addr] / 2;

		}
	}

	w = width;
	h = height;

	return 0;
}

void InitTrees() {
	
	TreeOne = new BTT();
	TreeTwo = new BTT();

}

void InitBuffers() {

	glGenBuffers(1, &vbID);
	glBindBuffer(GL_ARRAY_BUFFER, vbID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

}

int maxDepth = 12;
int ReadVerticesFromBTT( BTT *tri, float apexX, float apexY, float leftX, float leftY, float rightX, float rightY, int depth) {

	tri->Split();

	if (depth >= maxDepth) {

		int addr;

		addr = (((apexY-1)*width) + (apexX-1))*4;
		vertices[numPoints+0] = glm::vec3(apexX, (int)pixels[addr], apexY);
		addr = (((rightY-1)*width) + (rightX-1))*4;
		vertices[numPoints+1] = glm::vec3(rightX, (int)pixels[addr], rightY);

		addr = (((rightY-1)*width) + (rightX-1))*4;		
		vertices[numPoints+2] = glm::vec3(rightX, (int)pixels[addr], rightY);
		addr = (((leftY-1)*width) + (leftX-1))*4;
		vertices[numPoints+3] = glm::vec3(leftX, (int)pixels[addr], leftY);

		addr = (((leftY-1)*width) + (leftX-1))*4;
		vertices[numPoints+4] = glm::vec3(leftX, (int)pixels[addr], leftY);
		addr = (((apexY-1)*width) + (apexX-1))*4;
		vertices[numPoints+5] = glm::vec3(apexX, (int)pixels[addr], apexY);

		numPoints+=6;

	} else {

		float centerX = (leftX+rightX)/2; 
		float centerY = (leftY+rightY)/2;

		ReadVerticesFromBTT(tri->left, centerX, centerY, apexX, apexY, leftX, leftY, depth+1);
		ReadVerticesFromBTT(tri->right, centerX, centerY, rightX, rightY, apexX, apexY, depth+1);

	} 

	return 1;
}

void Render() {

	GLuint ModelUniform = glGetUniformLocation(ShaderId, "model");
	GLuint ViewUniform = glGetUniformLocation(ShaderId, "view");
	GLuint ProjectionUniform = glGetUniformLocation(ShaderId, "projection");
	GLuint VertexColorUniform = glGetUniformLocation(ShaderId, "vertexColor");

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-w/2, 0, 0));
	model = glm::translate(model, glm::vec3(0, 0, -h/2));
	glUniformMatrix4fv(ModelUniform, 1, GL_FALSE, &model[0][0]);

	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 500.0f, -500.0f),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);
	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, &view[0][0]);

	glm::mat4 projection = glm::perspective(
		75.0f,			// The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
		4.0f / 3.0f,	// Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
		0.1f,			// Near clipping plane. Keep as big as possible, or you'll get precision issues.
		10000000.0f     // Far clipping plane. Keep as little as possible.
	);

	// Whats wrong with this?
	// glm::mat4 projection( glm::ortho(0, width, height, 0, 0, 100) );
	glUniformMatrix4fv(ProjectionUniform, 1, GL_FALSE, &projection[0][0]);

	//glUniformMatrix4fv(LightPositionUniform, 1, GL_FALSE, &model[0][0]);
	glUniform4f(VertexColorUniform, 0.0f, 0.0f, 0.0f, 1.0f);

	numPoints = 0;
	ReadVerticesFromBTT(TreeOne, 0, 0, w, 0, 0, h, 0); 
	ReadVerticesFromBTT(TreeTwo, w, h, 0, h, w, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbID);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // each vertex consists of 3 floats
		GL_FLOAT,           // each value is a float
		GL_FALSE,           // normalized?
		0,                  // stride; dont skip over any data
		(void*)0            // start at the first element in the array
	);

	glDrawArrays(GL_LINES, 0, numPoints);

	glDisableVertexAttribArray(0);
}

int main() {

	if (!InitializeWindow()) {
		return 1;	
	}

	LoadHeightmap();
	InitTrees();

	// Initializes random number seed.
	srand(time(NULL));

	InitBuffers();

	while (GLFW_OPENED) {

		inputs.GLFWGetInputs();
		if ( inputs.KeyPressed(GLFW_KEY_ESC) ) break;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Render();

		glfwSwapBuffers();
	}
}
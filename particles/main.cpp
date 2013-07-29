//#include <stdio.h>
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

GLfloat points[1000];
GLfloat pointColors[1000];
int numPoints = 0;
GLuint ShaderId;
GLuint vbID;
GLuint cID;
GLuint texId;

#include "input.h"
#include "particles.h"

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

GLuint loadTGA_glfw(const char * imagepath){
 
    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
 
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
 
    // Read the file, call glTexImage2D with the right parameters
    if (glfwLoadTexture2D(imagepath, GLFW_ALPHA_MAP_BIT) == GL_FALSE) return 0;

    // Nice trilinear filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
 
    // Return the ID of the texture we just created
    return textureID;
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
 
	glfwSetWindowTitle( "A* Pathfinding" );

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

	// Disables backface culling; needed for transparency?
	//glDisable(GL_CULL_FACE);
	//glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//GLuint VertexArrayID;
	//glGenVertexArrays(1, &VertexArrayID);
	//glBindVertexArray(VertexArrayID);

	cout << "\n\nError: " << glGetError();

	char thePath[256];
	_getcwd(thePath, 255);
	cout << "\nWorking Directory: \n" << thePath;

	texId = loadTGA_glfw("particle.tga");
	if (!texId) return 0;

	return true;
}

void InitParticles() {

	glPointSize(50);

	glGenBuffers(1, &vbID);
	glBindBuffer(GL_ARRAY_BUFFER, vbID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	glGenBuffers(1, &cID);
	glBindBuffer(GL_ARRAY_BUFFER, cID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointColors), pointColors, GL_STATIC_DRAW);
}

void Render() {

	GLuint ModelUniform = glGetUniformLocation(ShaderId, "model");
	GLuint ProjectionUniform = glGetUniformLocation(ShaderId, "projection");
	GLuint VertexColorUniform = glGetUniformLocation(ShaderId, "vertexColor");

	glm::mat4 model(1.0f);
	glUniformMatrix4fv(ModelUniform, 1, GL_FALSE, &model[0][0]);

	glm::mat4 projection = glm::mat4( glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT), 0.0f, -1000.0f, 1000.0f) );
	// Whats wrong with this?
	// glm::mat4 projection( glm::ortho(0, width, height, 0, 0, 100) );
	glUniformMatrix4fv(ProjectionUniform, 1, GL_FALSE, &projection[0][0]);

	// color
	glUniform3f(VertexColorUniform, 1.0f, 0.0f, 0.0f);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbID);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // each vertex consists of 2 floats
		GL_FLOAT,           // each value is a float
		GL_FALSE,           // normalized?
		0,                  // stride; dont skip over any data
		(void*)0            // start at the first element in the array
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, cID);
	glVertexAttribPointer(
		1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
		3,                  // each vertex consists of 3 floats
		GL_FLOAT,           // each value is a float
		GL_FALSE,           // normalized?
		0,                  // stride; dont skip over any data
		(void*)0            // start at the first element in the array
	);

	//int pointCount = (sizeof(points)/sizeof(points[0])) / 3;

	glDrawArrays(GL_POINTS, 0, numPoints);

	glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
}

int main() {

	if (!InitializeWindow()) {
		return 1;	
	}

	// Initializes random number seed.
	srand(time(NULL));

	InitParticles();
	Particles* p = new Particles();

	int mouseX = 0;
	int mouseY = 0;

	double lastTime = 0;

	double particleInterval = 0.10;
	int maxPoints = sizeof(points)/sizeof(points[0])/3;

	while (GLFW_OPENED) {

		//cout << 

		//if ( ! lastTime) lastTime = currentTime;
		//deltaTime = currentTime - lastTime;
		//lastTime = currentTime;

		//curAnimTime += deltaTime;
		//if (curAnimTime > maxAnimTime) {
		//	curAnimTime -= maxAnimTime;
		//}

		inputs.GLFWGetInputs();
		glfwGetMousePos(&mouseX, &mouseY);

		if ( inputs.KeyPressed(GLFW_KEY_ESC) ) break;
		if ( inputs.MousePressed(GLFW_MOUSE_BUTTON_1) ) {}

		double currentTime = glfwGetTime();
		int numParticlesToAdd = floor((currentTime-lastTime)/particleInterval);

		p->Update(currentTime - lastTime);

		/*
		for (unsigned int i = 0; i < numParticlesToAdd; ++i) {

			numPoints++;
			if (numPoints == maxPoints) return 0;

			points[numPoints*3] = rand()%WINDOW_WIDTH;
			points[(numPoints*3)+1] = rand()%WINDOW_HEIGHT;
			points[(numPoints*3)+2] = numPoints*0.01;

		}
		*/
		
		vector<Point> particles = p->GetPoints();
		numPoints = particles.size();

		//if (numPoints > 0) numPoints = 1;

		//cout << "\nNUM: " << numPoints;

		for (unsigned int i = 0; i < numPoints; ++i) {
			points[i*3]			= particles[i].x;
			points[(i*3)+1]		= particles[i].y;
			points[(i*3)+2]		= particles[i].z;

			//cout << "\nX: " << points[i*3];
			//cout << "\nY: " << points[(i*3)+1];
			//cout << "\nZ: " << points[(i*3)+2];

			pointColors[i*3]		= particles[i].color[0];
			pointColors[(i*3)+1]	= particles[i].color[1];
			pointColors[(i*3)+2]	= particles[i].color[2];

			//cout << "\nR: " << pointColors[i*3];
			//cout << "\nG: " << pointColors[(i*3)+1];
			//cout << "\nB: " << pointColors[(i*3)+2];
			//cout << "\n";
		}

		//cout << "\nNumPoints: " << p->NumPoints() << "\n";

		glBindBuffer(GL_ARRAY_BUFFER, vbID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), &points[0]);

		glBindBuffer(GL_ARRAY_BUFFER, cID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointColors), &pointColors[0]);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Render();

		glfwSwapBuffers();

		//lastTime = lastTime + (numParticlesToAdd*particleInterval);
		lastTime = currentTime;
	}
}
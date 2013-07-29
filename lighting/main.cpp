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

/*GLfloat vertices[36] = {
	-1, 0, 0, 
	0, 0, -1,
	0, 1, 0,

	0, 0, -1,
	1, 0, 0,
	0, 1, 0,

	1, 0, 0,
	0, 0, 1,
	0, 1, 0,

	0, 0, 1,
	-1, 0, 0,
	0, 1, 0
};*/

glm::vec3 vertices[] = {
	glm::vec3(-1, 0, 0),
	glm::vec3(0, 0, -1),
	glm::vec3(0, 1, 0),
	glm::vec3(1, 0, 0),
	glm::vec3(0, 0, 1),
};

unsigned short indices[] = {
	2, 1, 0,
	2, 3, 1,
	2, 4, 3,
	2, 0, 4
};

std::vector<glm::vec3> normals( sizeof(vertices)/sizeof(vertices[0]) );

//glm::vec3 normals[36];

GLuint ShaderId;
GLuint vbID;
GLuint inID;
GLuint nbID;
const int frameRate = 30;

#include "input.h"

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
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//GLuint VertexArrayID;
	//glGenVertexArrays(1, &VertexArrayID);
	//glBindVertexArray(VertexArrayID);

	cout << "\n\nError: " << glGetError();

	char thePath[256];
	_getcwd(thePath, 255);
	cout << "\nWorking Directory: \n" << thePath;

	return true;
}

void InitBuffers() {
	glGenBuffers(1, &vbID);
	glBindBuffer(GL_ARRAY_BUFFER, vbID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &inID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, inID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0] , GL_STATIC_DRAW);

	glGenBuffers(1, &nbID);
	glBindBuffer(GL_ARRAY_BUFFER, nbID);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
}

glm::mat4 model(1.0f);

float mRotationY = 0.0f;
float mTranslateX = 0.0f;
float lightX = 0.0f;
float lightY = 2.0f;
float lightZ = 0.30f;
double lastTime = 0.0f;

void Render() {

	GLuint ModelUniform = glGetUniformLocation(ShaderId, "model");
	GLuint ViewUniform = glGetUniformLocation(ShaderId, "view");
	GLuint ProjectionUniform = glGetUniformLocation(ShaderId, "projection");
	GLuint VertexColorUniform = glGetUniformLocation(ShaderId, "vertexColor");
	GLuint LightPositionUniform = glGetUniformLocation(ShaderId, "lightPosition");

	glm::mat4 view = glm::lookAt(
		glm::vec3(0, 1.5f, -1.5f),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);
	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, &view[0][0]);

	glm::mat4 projection = glm::perspective(
		75.0f,         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
		4.0f / 3.0f, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
		0.1f,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
		100.0f       // Far clipping plane. Keep as little as possible.
	);
	// Whats wrong with this?
	// glm::mat4 projection( glm::ortho(0, width, height, 0, 0, 100) );
	glUniformMatrix4fv(ProjectionUniform, 1, GL_FALSE, &projection[0][0]);

	double time = glfwGetTime();
	double elapsedTime = time - lastTime;
	lastTime = time;

	//lightY -= elapsedTime * 0.3f;
	//if (lightY <= 0) lightY = 2.0f;

	//cout << "\n" << lightY;

	model = glm::mat4(1.0f);

	if (inputs.KeyDown(GLFW_KEY_UP)) lightY += elapsedTime * 0.8f;
	if (inputs.KeyDown(GLFW_KEY_DOWN)) lightY -= elapsedTime * 0.8f;

	float xSpeed = 0.6f;

	if (inputs.KeyDown(GLFW_KEY_RIGHT)) {
		//lightX += elapsedTime * 0.8f;
		mTranslateX -= xSpeed * elapsedTime;
	}

	if (inputs.KeyDown(GLFW_KEY_LEFT)) {
		//lightX -= elapsedTime * 0.8f;
		mTranslateX += xSpeed * elapsedTime;
	}

	mRotationY += 45.0f * elapsedTime;
	if (mRotationY >= 360.0f) mRotationY = 0.0f;

	model = glm::translate(model, glm::vec3(mTranslateX, 0.0f, 0.0f));
	model = glm::rotate(model, mRotationY, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(ModelUniform, 1, GL_FALSE, &model[0][0]);

	//glUniform3f(LightPositionUniform, 0.0f, 0.3f, -0.5f);
	glUniform3f(LightPositionUniform, lightX, lightY, lightZ);

	//glUniformMatrix4fv(LightPositionUniform, 1, GL_FALSE, &model[0][0]);
	glUniform4f(VertexColorUniform, 1.0f, 0.0f, 0.0f, 1.0f);

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

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, nbID);
	glVertexAttribPointer(
		1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
		3,                  // each vertex consists of 3 floats
		GL_FLOAT,           // each value is a float
		GL_TRUE,           // normalized?
		0,                  // stride; dont skip over any data
		(void*)0            // start at the first element in the array
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, inID);

	//glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices)/sizeof(vertices[0]));
	glDrawElements(
		GL_TRIANGLES,      // mode
		sizeof(indices)/sizeof(indices[0]),    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

int main() {

	if (!InitializeWindow()) {
		return 1;	
	}

	std::vector<glm::vec3> triangleNormals[ (sizeof(vertices)/sizeof(vertices[0])) ];
	for (unsigned int i = 0; i < (sizeof(indices)/sizeof(indices[0])); i+=3) {
		glm::vec3 v1 = vertices[indices[i]];
		glm::vec3 v2 = vertices[indices[i+1]];
		glm::vec3 v3 = vertices[indices[i+2]];

		glm::vec3 e1 = v2-v1;
		glm::vec3 e2 = v3-v1;
		glm::vec3 triNormal = glm::normalize(glm::cross(e1, e2));

		cout << "\nV" << i << ") X: " << triNormal.x << " Y: " << triNormal.y << " Z: " << triNormal.z;

		triangleNormals[indices[i]].push_back(triNormal);
		triangleNormals[indices[i+1]].push_back(triNormal);
		triangleNormals[indices[i+2]].push_back(triNormal);
	}

	cout << "\n";

	for (unsigned int i = 0; i < (sizeof(vertices)/sizeof(vertices[0])); ++i) {

		for (unsigned int k = 0; k < triangleNormals[i].size(); ++k) {
			normals[i] += triangleNormals[i][k];
		}
		normals[i] /= triangleNormals[i].size();

	}

	//for (unsigned int i = 0; i < (sizeof(in)/sizeof(in[0])); ++i) {

	//}

	//for (unsigned int i = 0; i < triangleNormals[i].size(); ++i) {

	//}

	for (unsigned int i = 0; i < normals.size(); ++i) {
		cout << "\nV" << i << ") X: " << normals[i].x << " Y: " << normals[i].y << " Z: " << normals[i].z;
	}

	InitBuffers();

	// Initializes random number seed.
	srand(time(NULL));

	while (GLFW_OPENED) {

		double currentTime = glfwGetTime();

		inputs.GLFWGetInputs();
		if ( inputs.KeyPressed(GLFW_KEY_ESC) ) break;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Render();

		glfwSwapBuffers();

	}
}
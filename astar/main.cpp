//#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include <map>
using namespace std;

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#define OCCUPIED 0
#define WALKABLE 1
#define START 2
#define FINISH 3
#define PATH 4
#define CHECKED 5

int Board[15][15];
GLuint ShaderId;

const int BoardXSize = sizeof(Board)/sizeof(Board[0]);
const int BoardYSize = sizeof(Board[0])/sizeof(Board[0][0]);
const int BoardSize = BoardXSize * BoardYSize;

int tileWidth = WINDOW_WIDTH/BoardXSize;
int tileHeight = WINDOW_HEIGHT/BoardYSize;

#include "pathfinder.h"
#include "input.h"
#include "square.h"

Square* square = NULL;

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

void RandomizeBoard() {
	for (unsigned int i = 0; i < sizeof(Board)/sizeof(Board[0]); ++i) {
		for (unsigned int k = 0; k < sizeof(Board[0])/sizeof(Board[0][0]); ++k) {
			Board[i][k] = rand()%2;
		}
	}
}

void DesignateTile(int x, int y, int designation) {
	//clear the designation from the board before assigning it to a new tile
	for (unsigned int i = 0; i < BoardXSize; ++i) {
		for (unsigned int k = 0; k < BoardYSize; ++k) {
			if (Board[i][k] == designation) Board[i][k] = WALKABLE;
		}
	}
	Board[x][y] = designation;
}

void SetTile(int x, int y, int designation) {
	if (Board[x][y] != START && Board[x][y] != FINISH) Board[x][y] = designation;
}

void RenderBoard() {

	for (unsigned int i = 0; i < BoardXSize; ++i) {
		for (unsigned int k = 0; k < BoardYSize; ++k) {

			glm::vec4 color;

			int tile = Board[i][k];
			if (tile == OCCUPIED) color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);	//green
			if (tile == WALKABLE) color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);	//black
			if (tile == START) color = glm::vec4(0.0f, 0.5f, 0.5f, 1.0f);		//teal
			if (tile == FINISH) color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);		//yellow
			if (tile == PATH) color = glm::vec4(1.0f, 0.71f, 0.83f, 1.0f);		//pink
			if (tile == CHECKED) color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);		//grey

			int x = 0 + (i*tileWidth);
			int y = 0 + (k*tileHeight);

			square->Draw(x, y, color);
		}
	}
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
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

	//GLuint VertexArrayID;
	//glGenVertexArrays(1, &VertexArrayID);
	//glBindVertexArray(VertexArrayID);

	cout << "\n\nError: " << glGetError();

	return true;
}

int main() {

	if (!InitializeWindow()) {
		return 1;	
	}

	square = new Square(tileWidth-2, tileHeight-2);
	RandomizeBoard();

	int StartX = 5;
	int StartY = 5;
	int EndX = 10;
	int EndY = 10;

	DesignateTile(StartX, StartY, START);
	DesignateTile(EndX, EndY, FINISH);

	int mouseX = 0;
	int mouseY = 0;

	while (GLFW_OPENED) {
		inputs.GLFWGetInputs();
		glfwGetMousePos(&mouseX, &mouseY);
		int mouseTileX = mouseX/tileWidth;
		int mouseTileY = mouseY/tileHeight;

		if ( inputs.KeyPressed(GLFW_KEY_ESC) ) break;

		if ( inputs.KeyPressed(GLFW_KEY_ENTER) ) {
			RandomizeBoard();
			StartX = 5;
			StartY = 5;
			EndX = 10;
			EndY = 10;	
			DesignateTile(StartX, StartY, START);
			DesignateTile(EndX, EndY, FINISH);
		}

		if ( inputs.KeyPressed(GLFW_KEY_SPACE) ) {
			Path path = FindPath(Board, StartX, StartY, EndX, EndY);

			for (unsigned int i = 0; i < BoardXSize; ++i) {
				for (unsigned int k = 0; k < BoardYSize; ++k) {
					if (Board[i][k] == PATH) Board[i][k] = WALKABLE;
				}
			}

			for (unsigned int i = 0; i < path.checkedCoords.size()/2; ++i) {
				int coordX = path.checkedCoords[i*2];
				int coordY = path.checkedCoords[(i*2)+1];
				if ( (coordX != StartX && coordX != EndX) && (coordY != StartY && coordY != EndY) ) {
					Board[coordX][coordY] = CHECKED;
				}
			}

			for (unsigned int i = 0; i < path.coords.size()/2; ++i) {
				int coordX = path.coords[i*2];
				int coordY = path.coords[(i*2)+1];
				cout << "\nX: " << coordX << " Y: " << coordY;
				Board[coordX][coordY] = PATH;
			}
		}

		if ( inputs.MousePressed(GLFW_MOUSE_BUTTON_1) ) {
			if ( inputs.KeyDown(GLFW_KEY_LCTRL) ) {
				StartX = mouseTileX;
				StartY = mouseTileY;
				DesignateTile(StartX, StartY, START);
			} else {
				SetTile(mouseTileX, mouseTileY, OCCUPIED);
			}
		}

		if ( inputs.MousePressed(GLFW_MOUSE_BUTTON_2) ) {
			if ( inputs.KeyDown(GLFW_KEY_LCTRL) ) {
				EndX = mouseTileX;
				EndY = mouseTileY;
				DesignateTile(EndX, EndY, FINISH);
			} else {
				SetTile(mouseTileX, mouseTileY, WALKABLE);
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderBoard();

		glfwSwapBuffers();
	}
}
class GLFWInput {

public:

	map<int, bool> keyPressed;
	map<int, bool> keyDown;
	map<int, bool> mousePressed;

	GLFWInput(){}
	~GLFWInput(){}

	void GLFWGetInputs() {

		int keys[] = {
			GLFW_KEY_ESC,
			GLFW_KEY_ENTER,
			GLFW_KEY_SPACE,
			GLFW_KEY_LCTRL,
			GLFW_KEY_UP,
			GLFW_KEY_DOWN,
			GLFW_KEY_LEFT,
			GLFW_KEY_RIGHT
		};

		for (unsigned int i = 0; i < sizeof(keys)/sizeof(keys[0]); ++i) {

			int key = keys[i];

			bool down = glfwGetKey(key);
			if (down) {
				if (keyDown[key]) {
					keyPressed[key] = false;
				} else {
					keyPressed[key] = true;
					keyDown[key]	= true;
				}
			} else {
				keyPressed[key]		= false;
				keyDown[key]		= false;
			}
		}

		int mouseButtons[] = {
			GLFW_MOUSE_BUTTON_1,
			GLFW_MOUSE_BUTTON_2
		};

		for (unsigned int i = 0; i < sizeof(mouseButtons)/sizeof(mouseButtons[0]); ++i) {
			mousePressed[mouseButtons[i]] = glfwGetMouseButton(mouseButtons[i]);
		}
	}

	bool KeyPressed(int id) {
		return keyPressed[id];
	}

	bool KeyDown(int id) {
		return keyDown[id];
	}

	bool MousePressed(int id) {
		return mousePressed[id];
	}
};

GLFWInput inputs = GLFWInput();
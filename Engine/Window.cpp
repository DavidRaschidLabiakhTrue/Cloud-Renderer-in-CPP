#include "Window.hpp"

unsigned int Window::ScreenWidth = 1600;
unsigned int Window::ScreenHeight = 900;

Camera * Window::camera = 0;
bool Window::keyBools[10] = { false, false,false, false, false, false, false, false, false, false };
bool Window::wireframe = false;
bool Window::firstMouse = true;

vec2 Window::last = vec2((float)Window::ScreenWidth / 2.0f, (float)Window::ScreenHeight / 2.0f);


bool Window::mouseCursorDisabled = true;

Window::Window(unsigned int scrW, unsigned int scrH, std::string name) : windowName(name)
{

	Window::ScreenWidth = scrW;
	Window::ScreenHeight = scrH;

	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	// glfw window creation										 
	this->WindowPointer = glfwCreateWindow(ScreenWidth, ScreenHeight, name.c_str(), NULL, NULL);
	if (!this->WindowPointer)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		successfulLoad = false;
		return;
	}
	glfwMakeContextCurrent(this->WindowPointer);
	glfwSetFramebufferSizeCallback(this->WindowPointer, &Window::framebuffer_size_callback);
	glfwSetCursorPosCallback(this->WindowPointer, &Window::mouse_callback);
	glfwSetScrollCallback(this->WindowPointer, &Window::scroll_callback);
	oldState = newState = GLFW_RELEASE;
	


	successfulLoad = gladLoader();
	if (successfulLoad)
	{
		std::cout << "GLFW window correctly initialized!" << std::endl;
	}
}

int Window::gladLoader() 
{

	// tell GLFW to capture our mouse
	glfwSetInputMode(this->WindowPointer, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return 0;
	}

	return 1;
}

// glfw: whenever the mouse moves, this callback is called
void Window::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		Window::last.x = xpos;
		Window::last.y = ypos;

		firstMouse = false;
	}

	float xoffset = xpos - Window::last.x;
	float yoffset = Window::last.y - ypos; // reversed since y-coordinates go from bottom to top

	last.x = xpos;
	last.y = ypos;

	if (!mouseCursorDisabled)
	{
		Window::camera->ProcessMouseMovement(xoffset, yoffset);
	}
		
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Window::camera->ProcessMouseScroll(yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
 
void Window::processInput(float frameTime) 
{
	if (glfwGetKey(this->WindowPointer, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->WindowPointer, true);

	if (glfwGetKey(this->WindowPointer, GLFW_KEY_W) == GLFW_PRESS)
		camera->ProcessKeyboard(FORWARD, frameTime);
	if (glfwGetKey(this->WindowPointer, GLFW_KEY_S) == GLFW_PRESS)
		camera->ProcessKeyboard(BACKWARD, frameTime);
	if (glfwGetKey(this->WindowPointer, GLFW_KEY_A) == GLFW_PRESS)
		camera->ProcessKeyboard(LEFT, frameTime);
	if (glfwGetKey(this->WindowPointer, GLFW_KEY_D) == GLFW_PRESS)
		camera->ProcessKeyboard(RIGHT, frameTime);
	
	newState = glfwGetMouseButton(this->WindowPointer, GLFW_MOUSE_BUTTON_RIGHT);

	if (newState == GLFW_RELEASE && oldState == GLFW_PRESS) {
		glfwSetInputMode(this->WindowPointer, GLFW_CURSOR, (mouseCursorDisabled
		? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL));
		mouseCursorDisabled = !mouseCursorDisabled;
		if (mouseCursorDisabled)
			firstMouse = true;
		//std::cout << "MOUSE R PRESSED!" << std::endl;
	}

	oldState = newState;
	
	// WIREFRAME
	if (glfwGetKey(this->WindowPointer, GLFW_KEY_T) == GLFW_PRESS) {
		if (keyBools[4] == false) {
			//std::cout << "WIREFRAME" << std::endl;
			wireframe = !wireframe;
			keyBools[4] = true;
		}
	}
	else if (glfwGetKey(this->WindowPointer, GLFW_KEY_T) == GLFW_RELEASE) {
		if (keyBools[4] == true) { keyBools[4] = false; } // Non aggiungere niente qui
	}
}

Window::~Window()
{
	this->terminate();
}

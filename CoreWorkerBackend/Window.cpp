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

	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

										 
	this->WindowPointer = glfwCreateWindow(ScreenWidth, ScreenHeight, name.c_str(), NULL, NULL);
	if (!this->WindowPointer)
	{
		std::cout << "GLFW Window Pointer Failure" << std::endl;
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
		std::cout << "GLFW Initialized" << std::endl;
	}
}

int Window::gladLoader() 
{

	
	glfwSetInputMode(this->WindowPointer, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "GLAD Initialized" << std::endl;
		return 0;
	}

	std::string vendor = reinterpret_cast<char const*>(glGetString(GL_VENDOR));
	std::string renderer = reinterpret_cast<char const*>(glGetString(GL_RENDERER));
	std::string version = reinterpret_cast<char const*>(glGetString(GL_VERSION));
	std::string shaderVersion = reinterpret_cast<char const*>(glGetString(GL_SHADING_LANGUAGE_VERSION));


	using std::cout;

	cout << "Vendor: " << vendor << std::endl;
	cout << "Renderer: " << renderer << std::endl;
	cout << "Version: " << version << std::endl;
	cout << "Shader Version: " << shaderVersion << std::endl;
	return 1;
}


void Window::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		Window::last.x = xpos;
		Window::last.y = ypos;

		firstMouse = false;
	}

	float xoffset = xpos - Window::last.x;
	float yoffset = Window::last.y - ypos; 

	last.x = xpos;
	last.y = ypos;

	if (!mouseCursorDisabled)
	{
		Window::camera->ProcessMouseMovement(xoffset, yoffset);
	}
		
}


void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Window::camera->ProcessMouseScroll(yoffset);
}


void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
 
void Window::processInput(float frameTime) 
{
	if (glfwGetKey(this->WindowPointer, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->WindowPointer, true);
	}

	if (glfwGetKey(this->WindowPointer, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera->ProcessKeyboard(CameraForwardDirection, frameTime);
	}
	if (glfwGetKey(this->WindowPointer, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera->ProcessKeyboard(CameraLeftDirection, frameTime);
	}
	if (glfwGetKey(this->WindowPointer, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera->ProcessKeyboard(CameraBackwardDirection, frameTime);
	}
	if (glfwGetKey(this->WindowPointer, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera->ProcessKeyboard(CameraRightDirection, frameTime);
	}
		
	
	newState = glfwGetMouseButton(this->WindowPointer, GLFW_MOUSE_BUTTON_RIGHT);

	if (newState == GLFW_RELEASE && oldState == GLFW_PRESS) 
	{
		glfwSetInputMode(this->WindowPointer, GLFW_CURSOR, (mouseCursorDisabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL));
		mouseCursorDisabled = !mouseCursorDisabled;
		if (mouseCursorDisabled)
		{
			firstMouse = true;
		}
	}

	oldState = newState;
	
}

Window::~Window()
{
	this->terminate();
}

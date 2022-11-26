#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Camera.hpp>
#include <iostream>

using glm::vec2;

class Window
{
	public:
		//initializer

		Window(unsigned int screenWidth = 1600, unsigned int screenHeight = 900, std::string name = "Volumetric Cloud Visualizer");
		~Window();
		GLFWwindow * WindowPointer;
		GLFWwindow * getWindow() const { return WindowPointer; }

		void processInput(float frameTime); 

		static unsigned int ScreenWidth;
		static unsigned int ScreenHeight;

		void terminate() 
		{
			glfwTerminate();
		}

		bool isWireframeActive() 
		{
			return Window::wireframe;
		}

		
		bool continueLoop() 
		{
			return !glfwWindowShouldClose(this->WindowPointer);
		}

		
		void swapBuffersAndPollEvents() 
		{
			glfwSwapBuffers(this->WindowPointer);
			glfwPollEvents();
		}

		static Camera * camera;
		bool successfulLoad;

	private:
		int oldState, newState;
		int gladLoader(); 
		std::string windowName;
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static bool mouseCursorDisabled;
		static bool keyBools[10];
		static bool wireframe;


		static bool firstMouse;
		static vec2 last;


		
};



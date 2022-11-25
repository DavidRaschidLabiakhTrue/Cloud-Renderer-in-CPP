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
		Window(unsigned int SCR_WIDTH = 1600, unsigned int SCR_HEIGHT = 900, std::string name = "Volumetric Cloud Visualizer");
		~Window();
		GLFWwindow * WindowPointer;
		GLFWwindow * getWindow() const { return WindowPointer; }

		void processInput(float frameTime); //input handler

		// screen settings
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

		// return if the main loop must continue
		bool continueLoop() 
		{
			return !glfwWindowShouldClose(this->WindowPointer);
		}

		//put this at the end of the main
		void swapBuffersAndPollEvents() 
		{
			glfwSwapBuffers(this->WindowPointer);
			glfwPollEvents();
		}

		static Camera * camera;
		bool successfulLoad;

	private:
		int oldState, newState;
		int gladLoader(); // set mouse input and load opengl functions 
		std::string windowName;
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

		static bool mouseCursorDisabled;
		
		static bool keyBools[10];

		

		// wireframe mode
		static bool wireframe;

		//avoid to make the mouse to jump at the start of the program
		static bool firstMouse;// = true;
		static vec2 last;


		
};



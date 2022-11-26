#pragma once
#ifndef GLAD_H
#define GLAD_H
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

#include "CoreWorkerBackend/Window.hpp"
#include "CoreWorkerBackend/Shader.hpp"
#include "CoreWorkerBackend/PostProcess.hpp"
#include "CoreWorkerBackend/Texture.hpp"

#include "Renderable/VolumetricClouds.hpp"
#include "Renderable/ReflectablePlane.hpp"
#include "Renderable/Skybox.hpp"
#include "Renderable/PlaneCounter.hpp"
#include "Renderable/DrawableClouds.hpp"

#include <Camera.hpp>
#include "Include/stb/stb_image.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>


#include "Renderable/Scene.hpp"
#include "Renderable/Drawable.hpp"
#include "Renderable/GUI.hpp"

#include <iostream>
#include <vector>
#include <functional>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

class Main
{
	public:
		Main();
		~Main();

		int run();

		void postProcess1(const FrameBufferObject& fbo, VolumetricClouds& volumetricClouds);
		void postProcess2(const Scene& scene, const FrameBufferObject& fbo, VolumetricClouds& volumetricClouds);

		void clear1();
		void clear2();
		void clear3();

		Camera camera;
		Window window;
		GUI gui;

		mat4 proj;
		mat4 view;

		PostProcessor PostProcessing;
};
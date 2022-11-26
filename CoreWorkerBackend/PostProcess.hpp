#pragma once
#include "Shader.hpp"
#include <glad/glad.h>
#include "../Renderable/Drawable.hpp"

// refactors those shaders which write only into the screen space (e.g. post processing, ShaderToy.com), so only the fragment shader it's needed and the model is only two triangles
class PostProcessor : Drawable
{
	public:

		PostProcessor(const char * fragmentPath);
		~PostProcessor();

		Shader * const getShaderPtr() 
		{
			return shader;
		}

		Shader & const getShader() 
		{
			return *shader;
		}

		virtual void draw();
		static void drawQuad();

		static void disableTests() 
		{
			glDisable(GL_DEPTH_TEST);
		}

		static void enableTests() 
		{
			glEnable(GL_DEPTH_TEST);
		}



	private:
		Shader * shader;
		static unsigned int vao;
		static unsigned int vbo;
		static bool postProcessorInitialized;

		void initialize();
};


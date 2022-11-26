#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include "../CoreWorkerBackend/Shader.hpp"
#include "../CoreWorkerBackend/buffers.hpp"
#include <GLFW/glfw3.h>
#include "../CoreWorkerBackend/Window.hpp"
#include "Drawable.hpp"

// we must load counter information to the plane in order to get the space rendering
// This acts like a reflectable surface more or less in that it will redirect rays from the ray marching back to the clouds *only*.
class PlaneCounter : public Drawable
{
	public:
		PlaneCounter(glm::vec2 position, float scale, float height);
		virtual ~PlaneCounter();
		virtual void draw();
		unsigned int vao, vbo, ebo;
		float scale, height;
		virtual void setGui();
		void bindRefractionFBO();
		void bindReflectionFBO();
		void unbindFBO();

		void setPosition(glm::vec2 position, float scale, float height) 
		{
			glm::mat4 identity;
			glm::mat4 scaleMatrix = glm::scale(identity, glm::vec3(scale, scale, scale));
			glm::mat4 transMatrix = glm::translate(identity, glm::vec3(position.x, height, position.y));
			this->modelMatrix = transMatrix * scaleMatrix;
		}



		float getHeight() 
		{
			return height;
		}

	
		static const int FBOw = 1280;
		static const int FBOh = 720;

		FrameBufferObject const& getReflectionFBO() 
		{
			return *reflectionFBO;
		}
		void drawVertices();


		FrameBufferObject * reflectionFBO; // these effectively do nothing but help the clouds *be clouds*
		FrameBufferObject * refractionFBO; // these effectively do nothing but help the clouds *be clouds*
		glm::mat4 modelMatrix;
		Shader * shader;
};


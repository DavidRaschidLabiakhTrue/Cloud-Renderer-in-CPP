#include "PlaneCounter.hpp"
#include "Scene.hpp"
#include "ReflectablePlane.hpp"
#include "../imgui/imgui.h"
#include "../CoreWorkerBackend/Utils.hpp"

PlaneCounter::PlaneCounter(glm::vec2 position, float scale, float height): scale(scale), height(height)
{

	shader = new Shader("PlaneCounter");
	shader->attachShader("shaders/PlaneCounter.vert")->attachShader("shaders/PlaneCounter.frag")->linkPrograms();


	glm::mat4 identity;
	glm::mat4 scaleMatrix = glm::scale(identity, glm::vec3(scale, scale, scale));
	glm::mat4 transMatrix = glm::translate(identity, glm::vec3(position.x, height, position.y));
	this->modelMatrix = transMatrix * scaleMatrix;

	reflectionFBO = new FrameBufferObject(FBOw, FBOh);
	refractionFBO = new FrameBufferObject(FBOw, FBOh);

	initializePlaneVAO(2, ReflectablePlane::tileW, &vao, &vbo, &ebo);
}

void PlaneCounter::bindReflectionFBO() 
{
	reflectionFBO->bind();
}

void PlaneCounter::bindRefractionFBO() 
{
	refractionFBO->bind();
}

const int planeResolution = 2;

void PlaneCounter::drawVertices() 
{
	glBindVertexArray(vao);
	shader->use();
	glDrawElements(GL_TRIANGLES, (planeResolution - 1)*(planeResolution - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


void PlaneCounter::setGui()
{

}

void PlaneCounter::draw() 
{
	shader->use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shader->uploadMatrix4ToGPU("modelMatrix", modelMatrix);
	shader->uploadMatrix4ToGPU("gVP", Drawable::scene->proj * Drawable::scene->cam->GetViewMatrix());
	this->drawVertices();
	glDisable(GL_BLEND);
}

void PlaneCounter::unbindFBO() 
{
	unbindCurrentFrameBuffer(Window::ScreenWidth, Window::ScreenWidth);
}



PlaneCounter::~PlaneCounter()
{
}

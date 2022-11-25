#include "PlaneCounter.hpp"
#include "Scene.hpp"
#include "ReflectablePlane.hpp"
#include "../imgui/imgui.h"
#include "../Engine/Utils.hpp"

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

	initializePlaneVAO(2, ReflectablePlane::tileW, &planeVAO, &planeVBO, &planeEBO);
}

void PlaneCounter::bindReflectionFBO() 
{
	reflectionFBO->bind();
}

void PlaneCounter::bindRefractionFBO() 
{
	refractionFBO->bind();
}

const int res = 2;

void PlaneCounter::drawVertices() 
{
	glBindVertexArray(planeVAO);
	shader->use();
	glDrawElements(GL_TRIANGLES, (res - 1)*(res - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


void PlaneCounter::setGui()
{

}

void PlaneCounter::draw() 
{
	// draw water plane
	shader->use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Scene * se = Drawable::scene;

	//this->setPosition(glm::vec2(se->cam.Position[0], se->cam.Position[2]), scale, height);
	this->setHeight(height);

	shader->setMat4("modelMatrix", modelMatrix);
	shader->setMat4("gVP", se->projMatrix*se->cam->GetViewMatrix());

	shader->setVec3("u_LightColor", se->lightColor);
	shader->setVec3("u_LightPosition", se->lightPos);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflectionFBO->tex);
	shader->setInt("reflectionTex", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refractionFBO->tex);
	shader->setInt("refractionTex", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dudvMap);
	shader->setInt("waterDUDV", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	shader->setInt("normalMap", 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, refractionFBO->depthTex);
	shader->setInt("depthMap", 4);

	float waveSpeed = 0.25;
	float time = glfwGetTime();

	float moveFactor = waveSpeed * time;
	shader->setFloat("moveFactor", moveFactor);

	shader->setVec3("cameraPosition", se->cam->Position);

	//waterPlane->Draw(*shad);
	this->drawVertices();
	glDisable(GL_BLEND);
}

void PlaneCounter::unbindFBO() 
{
	unbindCurrentFrameBuffer(Window::SCR_WIDTH, Window::SCR_WIDTH);
}



PlaneCounter::~PlaneCounter()
{
}

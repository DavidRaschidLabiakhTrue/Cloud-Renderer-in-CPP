#ifndef SCENELEMENTS_H
#define SCENELEMENTS_H

#include <Camera.hpp>
#include <glm/glm.hpp>
#include "../Engine/buffers.hpp"
#include <random>

using glm::vec3;
using glm::mat4;



struct Scene 
{

	glm::vec3 lightPos, lightColor, lightDir, fogColor, seed;
	glm::mat4 projMatrix;
	Camera * cam;
	FrameBufferObject * sceneFBO;
	bool wireframe = false;

	vec3 altFog = vec3(.2);

	void loadLight(vec3 lightPosition, vec3 lightColor, vec3 lightDirection)
	{
		this->lightPos = lightPosition;
		this->lightColor = lightColor;
		this->lightDir = lightDirection;
	}
	void loadAttributes(vec3 fogColor, vec3 seed)
	{
		this->fogColor = vec3(.3);
		this->seed = seed;
	}
	void loadCameraData(Camera& camera, mat4 proj)
	{
		this->projMatrix = proj;
		this->cam = &camera;
	}

	void loadFrameBuffer(FrameBufferObject& fbo)
	{
		this->sceneFBO = &fbo;
	}

};

#endif
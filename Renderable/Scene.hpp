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
	glm::mat4 proj;
	Camera * cam;
	FrameBufferObject * sceneFBO;
	bool wireframe = false;

	vec3 altFog = vec3(.2);

	Scene& loadLight(vec3 lightPosition, vec3 lightColor, vec3 lightDirection)
	{
		this->lightPos = lightPosition;
		this->lightColor = lightColor;
		this->lightDir = lightDirection;

		return *this;
	}
	Scene& loadAttributes(vec3 fogColor, vec3 seed)
	{
		this->fogColor = vec3(.3);
		this->seed = seed;
		return *this;
	}
	Scene& loadCameraData(Camera& camera, mat4 proj)
	{
		this->proj = proj;
		this->cam = &camera;
		return *this;
	}

	Scene& loadFrameBuffer(FrameBufferObject& fbo)
	{
		this->sceneFBO = &fbo;
		return *this;
	}

	void updateCameraAttributes(float reflectedHeight)
	{
		this->cam->invertPitch();

		this->cam->cameraPosition.y -= 2 * (this->cam->cameraPosition.y - reflectedHeight);
	}

};

#endif
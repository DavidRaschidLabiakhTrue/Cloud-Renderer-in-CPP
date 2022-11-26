#pragma once

#include <glm/glm.hpp>
#include "../CoreWorkerBackend/PostProcess.hpp"
#include "Drawable.hpp"
#include <glm/gtc/matrix_transform.hpp>

struct PreloadedColor 
{
	glm::vec3 cloudColorBottom, skyColorTop, skyColorBottom, lightColor, fogColor;
};

// Handles the sky and its color 
// More of a projected sphere...boxes in practice are not fit for the dynamic colors we needed
class Skybox : public Drawable 
{
	public:
		
		Skybox();
		~Skybox();

		virtual void draw();
		virtual void setGui();
		virtual void update();



		void mixSkyColorPreset(float v, PreloadedColor p1, PreloadedColor p2);

		unsigned int getSkyTexture() 
		{
			return skyboxFBO->tex;
		}
		PreloadedColor DefaultPreset();
		PreloadedColor SunsetPreset();
		PreloadedColor SunsetPreset1();
		PreloadedColor presetSunset, highSunPreset;
		friend class VolumetricClouds;
	private:
		glm::vec3 skyColorTop, skyColorBottom;

		PostProcessor * skyboxShader;
		FrameBufferObject * skyboxFBO;

	
};


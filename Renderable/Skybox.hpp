#pragma once

#include <glm/glm.hpp>
#include "../CoreWorkerBackend/PostProcess.hpp"
#include "Drawable.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "../ProjectExtras/TimeOfDay.hpp"

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



		void mergedLoadedColors(float v, PreloadedColor p1, PreloadedColor p2);

		unsigned int getSkyTexture() 
		{
			return skyboxFBO->textureID;
		}

		PreloadedColor presetSunset, highSunPreset;
		friend class VolumetricClouds;

		TimeOfDayOperator timeOperator;

		glm::vec3 skyColorTop, skyColorBottom;

		PostProcessor * skyboxShader;
		FrameBufferObject * skyboxFBO;

	
};


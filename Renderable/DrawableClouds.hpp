#pragma once
#include <glm/glm.hpp>
#include "../CoreWorkerBackend/Shader.hpp"
#include "Scene.hpp"
#include "../CoreWorkerBackend/PostProcess.hpp"
#include "../imgui/imgui.h"
#include "Skybox.hpp"

#define INT_CEIL(n,d) (int)ceil((float)n/d)


class DrawableClouds : public Drawable
{
	public:
		friend class VolumetricClouds;

		DrawableClouds(Scene * scene, Skybox * sky);
		~DrawableClouds();

		virtual void update();
		virtual void setGui();
	
		
		virtual void draw() 
		{
			// this implementation is not real. lets us hook up class to GUI
		};


	private:

		void launchCloudShaders();
		void launchVariables();
		void generatingWeatherMapData();
		void generatingModelTexturesData();
		Shader * volumetricCloudsShader;
		PostProcessor* postProcessingShader;
		Shader * weatherShader;
		float perlinFrequency;
		bool postProcess;
		float cloudAbsorption;
		float cloudCoverage;
		float cloudCrispiness;
		float cloudCurliness;
		float cloudDensity;
		float cloudSpeed;

		
		unsigned int weatheringID;
		

		float spatialRadiusOfRenderSpace;
		float sphereInnerRadius;
		float sphereOuterRadius;


		unsigned int worleyID;
		unsigned int perlinID;
		glm::vec3 cloudColorTop;
		glm::vec3 cloudColorBottom;
		glm::vec3 seed;
		glm::vec3 oldSeed;


	
	
		

		Scene * scene;
		Skybox * sky;

};


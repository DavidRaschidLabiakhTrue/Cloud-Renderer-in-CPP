#pragma once
#include <glm/glm.hpp>
#include "../CoreWorkerBackend/Shader.hpp"
#include "Scene.hpp"
#include "../CoreWorkerBackend/PostProcess.hpp"
#include "../imgui/imgui.h"
#include "Skybox.hpp"

#define INT_CEIL(n,d) (int)ceil((float)n/d)

//CloudsModel is responsible to collect the attributes and shaders that will be needed to render the volumetric clouds. Also, it creates the noises which models the clouds.
class DrawableClouds : public Drawable
{
	public:
		friend class VolumetricClouds;

		DrawableClouds(Scene * scene, Skybox * sky);
		~DrawableClouds();

		virtual void update();
		virtual void setGui();
	
		// fake implementation, it's needed to let this class being a drawableObject to subscribe to GUI class. must be fixed
		virtual void draw() {};


	private:
		Shader * volumetricCloudsShader, *weatherShader;
		PostProcessor * postProcessingShader;

		float coverage, cloudSpeed, crispiness, curliness, density, absorption;
		float earthRadius, sphereInnerRadius, sphereOuterRadius;
		float perlinFrequency;
		bool enableGodRays;
		bool postProcess;

		glm::vec3 cloudColorTop, cloudColorBottom;

		glm::vec3 seed, oldSeed;
		unsigned int perlinID, worleyID, weatheringID;

		Scene * scene;
		Skybox * sky;
		void initShaders();
		void initVariables();
		void generateWeatherMap();
		void generateModelTextures();
	
		


};


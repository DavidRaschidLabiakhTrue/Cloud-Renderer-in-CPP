#include "DrawableClouds.hpp"
#include <glad/glad.h>
#include "../CoreWorkerBackend/Texture.hpp"



void DrawableClouds::setGui() {

	ImGui::Begin(" ");
	ImGui::TextColored(ImVec4(.2, .2, 1, 1), "Cloud controls");
	ImGui::Checkbox("Post Processor", &postProcess);


	ImGui::SliderFloat("Coverage", &cloudCoverage, 0.0f, 1.0f);
	ImGui::SliderFloat("Speed", &cloudSpeed, 0.0f, 5.0E3);
	ImGui::SliderFloat("Crispiness", &cloudCrispiness, 0.0f, 120.0f);
	ImGui::SliderFloat("Curliness", &cloudCurliness, 0.0f, 3.0f);
	ImGui::SliderFloat("Density", &cloudDensity, 0.0f, 0.1f);
	ImGui::SliderFloat("Light absorption", &cloudAbsorption, 0.0f, 1.5f);
	ImGui::SliderFloat("Sky to Earth Radial Factor", &spatialRadiusOfRenderSpace, 10000.0f, 5000000.0f);
	ImGui::SliderFloat("Start of Clouds", &sphereInnerRadius, 1000.0f, 15000.0f);
	ImGui::SliderFloat("End of Clouds", &sphereOuterRadius, 1000.0f, 40000.0f);

	if (ImGui::SliderFloat("Clouds frequency", &perlinFrequency, 0.0f, 4.0f))
	{
		generatingWeatherMapData();
	}
		

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Colors");
	glm::vec3 * cloudBottomColor = &cloudColorBottom;
	ImGui::ColorEdit3("Cloud color", (float*)cloudBottomColor); // Edit 3 floats representing a color

	ImGui::End();
}

DrawableClouds::DrawableClouds(Scene * scene, Skybox * sky) : scene(scene), sky(sky)
{
	launchVariables();
	launchCloudShaders();
	generatingModelTexturesData();
}

void DrawableClouds::launchCloudShaders()
{
	volumetricCloudsShader = new Shader("volumetricCloudsShader", "shaders/volumetric_clouds.comp");
	postProcessingShader = new PostProcessor("shaders/CloudsPostProcessor.frag");

	deteriorationShader = new Shader("deteriorate");
	deteriorationShader->attachShader("shaders/deteriorate.comp")->linkPrograms();
}

void DrawableClouds::generatingModelTexturesData()
{

	if (!perlinID) 
	{
		Shader comp("PerlinWorleyCompute");
		comp.attachShader("shaders/PerlinWorley.comp")->linkPrograms();

		this->perlinID = generateTexture3D(128, 128, 128);
		comp.use();
		comp.setVec3("u_resolution", glm::vec3(128, 128, 128));
		glActiveTexture(GL_TEXTURE0);
		comp.setInt("outVolTex", 0);
		glBindTexture(GL_TEXTURE_3D, this->perlinID);
	  	glBindImageTexture(0, this->perlinID, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
		glDispatchCompute(INT_CEIL(128, 4), INT_CEIL(128, 4), INT_CEIL(128, 4));

		glGenerateMipmap(GL_TEXTURE_3D);
	}

	if (!worleyID) 
	{
		Shader worleyComputer("WorleyCompute");
		worleyComputer.attachShader("shaders/Worley.comp")->linkPrograms();
		this->worleyID = generateTexture3D(32, 32, 32);
		worleyComputer.use();
		glActiveTexture(GL_TEXTURE0);
	  	glBindTexture(GL_TEXTURE_3D, this->worleyID);
	  	glBindImageTexture(0, this->worleyID, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
		glDispatchCompute(INT_CEIL(32, 4), INT_CEIL(32, 4), INT_CEIL(32, 4));
		glGenerateMipmap(GL_TEXTURE_3D);
	}

	if (!weatheringID) 
	{
		this->weatheringID = generateTexture2D(1024, 1024);
		generatingWeatherMapData();
		seed = scene->seed;
		oldSeed = seed;
	}
}

DrawableClouds::~DrawableClouds()
{
	delete volumetricCloudsShader;
	delete postProcessingShader;
	delete deteriorationShader;
}

void DrawableClouds::update()
{
	seed = scene->seed;
	if (seed != oldSeed) 
	{
		generatingWeatherMapData();
		oldSeed = seed;
	}
}

void DrawableClouds::generatingWeatherMapData() 
{
	bindTexture2D(weatheringID, 0);
	deteriorationShader->use();
	deteriorationShader->setVec3("seed", scene->seed);
	deteriorationShader->setFloat("perlinFrequency", perlinFrequency);
	glDispatchCompute(INT_CEIL(1024, 8), INT_CEIL(1024, 8), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void DrawableClouds::launchVariables()
{
	cloudSpeed = 450.0;
	cloudCoverage = 0.45;
	cloudCrispiness = 40.;
	cloudCurliness = .1;
	cloudDensity = 0.02;
	cloudAbsorption = 0.35;
	spatialRadiusOfRenderSpace = 600000.0;
	sphereInnerRadius = 5000.0;
	sphereOuterRadius = 17000.0;
	perlinFrequency = 0.8;
	postProcess = true;
	seed = glm::vec3(0.0, 0.0, 0.0);
	oldSeed = glm::vec3(0.0, 0.0, 0.0);
	cloudColorTop = (glm::vec3(169., 149., 149.)*(1.5f / 255.f));
	cloudColorBottom = (glm::vec3(65., 70., 80.)*(1.5f / 255.f));
	weatheringID = 0;
	perlinID = 0;
	worleyID = 0;
}

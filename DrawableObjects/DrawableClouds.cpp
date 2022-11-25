#include "DrawableClouds.hpp"
#include <glad/glad.h>
#include "../Engine/Texture.hpp"



void DrawableClouds::setGui() {

	ImGui::Begin(" ");
	ImGui::TextColored(ImVec4(.2, .2, 1, 1), "Cloud controls");
	ImGui::Checkbox("Post Processor", &postProcess);


	ImGui::SliderFloat("Coverage", &coverage, 0.0f, 1.0f);
	ImGui::SliderFloat("Speed", &cloudSpeed, 0.0f, 5.0E3);
	ImGui::SliderFloat("Crispiness", &crispiness, 0.0f, 120.0f);
	ImGui::SliderFloat("Curliness", &curliness, 0.0f, 3.0f);
	ImGui::SliderFloat("Density", &density, 0.0f, 0.1f);
	ImGui::SliderFloat("Light absorption", &absorption, 0.0f, 1.5f);
	ImGui::SliderFloat("Sky to Earth Radial Factor", &earthRadius, 10000.0f, 5000000.0f);
	ImGui::SliderFloat("Start of Clouds", &sphereInnerRadius, 1000.0f, 15000.0f);
	ImGui::SliderFloat("End of Clouds", &sphereOuterRadius, 1000.0f, 40000.0f);

	if (ImGui::SliderFloat("Clouds frequency", &perlinFrequency, 0.0f, 4.0f))
	{
		generateWeatherMap();
	}
		

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Colors");
	glm::vec3 * cloudBottomColor = &cloudColorBottom;
	ImGui::ColorEdit3("Cloud color", (float*)cloudBottomColor); // Edit 3 floats representing a color

	ImGui::End();
}

DrawableClouds::DrawableClouds(Scene * scene, Skybox * sky) : scene(scene), sky(sky)
{
	initVariables();
	initShaders();
	generateModelTextures();
}

void DrawableClouds::initShaders()
{
	volumetricCloudsShader = new Shader("volumetricCloudsShader", "shaders/volumetric_clouds.comp");
	postProcessingShader = new PostProcessor("shaders/clouds_post.frag");
	//compute shaders
	weatherShader = new Shader("weatherMap");
	weatherShader->attachShader("shaders/weather.comp");
	weatherShader->linkPrograms();
}

void DrawableClouds::generateModelTextures()
{
	/////////////////// TEXTURE GENERATION //////////////////
	if (!perlinID) 
	{
		//compute shaders
		Shader comp("perlinWorley");
		comp.attachShader("shaders/perlinworley.comp");
		comp.linkPrograms();

		//make texture
		this->perlinID = generateTexture3D(128, 128, 128);
		//compute
		comp.use();
		comp.setVec3("u_resolution", glm::vec3(128, 128, 128));
		std::cout << "computing perlinworley!" << std::endl;
		glActiveTexture(GL_TEXTURE0);
		comp.setInt("outVolTex", 0);
		glBindTexture(GL_TEXTURE_3D, this->perlinID);
	  	glBindImageTexture(0, this->perlinID, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
		glDispatchCompute(INT_CEIL(128, 4), INT_CEIL(128, 4), INT_CEIL(128, 4));
		std::cout << "computed!!" << std::endl;
		glGenerateMipmap(GL_TEXTURE_3D);
	}

	if (!worleyID) 
	{
		//compute shaders
		Shader worley_git("worleyComp");
		worley_git.attachShader("shaders/worley.comp");
		worley_git.linkPrograms();

		//make texture
		this->worleyID = generateTexture3D(32, 32, 32);

		//compute
		worley_git.use();
		//worley_git.setVec3("u_resolution", glm::vec3(32, 32, 32));
		glActiveTexture(GL_TEXTURE0);
	  	glBindTexture(GL_TEXTURE_3D, this->worleyID);
	  	glBindImageTexture(0, this->worleyID, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
		std::cout << "computing worley 32!" << std::endl;
		glDispatchCompute(INT_CEIL(32, 4), INT_CEIL(32, 4), INT_CEIL(32, 4));
		std::cout << "computed!!" << std::endl;
		glGenerateMipmap(GL_TEXTURE_3D);
	}




	if (!weatheringID) 
	{
		//make texture
		this->weatheringID = generateTexture2D(1024, 1024);

		//compute
		generateWeatherMap();

		seed = scene->seed;
		oldSeed = seed;
	}
}

DrawableClouds::~DrawableClouds()
{
	delete volumetricCloudsShader;
	delete postProcessingShader;
	delete weatherShader;
}

void DrawableClouds::update()
{
	seed = scene->seed;
	if (seed != oldSeed) {
		generateWeatherMap();
		oldSeed = seed;
	}
}

void DrawableClouds::generateWeatherMap() 
{
	bindTexture2D(weatheringID, 0);
	weatherShader->use();
	weatherShader->setVec3("seed", scene->seed);
	weatherShader->setFloat("perlinFrequency", perlinFrequency);
	std::cout << "computing weather!" << std::endl;
	glDispatchCompute(INT_CEIL(1024, 8), INT_CEIL(1024, 8), 1);
	std::cout << "weather computed!!" << std::endl;

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void DrawableClouds::initVariables()
{
	cloudSpeed = 450.0;
	coverage = 0.45;
	crispiness = 40.;
	curliness = .1;
	density = 0.02;
	absorption = 0.35;

	earthRadius = 600000.0;
	sphereInnerRadius = 5000.0;
	sphereOuterRadius = 17000.0;

	perlinFrequency = 0.8;

	enableGodRays = false;
	postProcess = true;

	seed = glm::vec3(0.0, 0.0, 0.0);
	oldSeed = glm::vec3(0.0, 0.0, 0.0);

	cloudColorTop = (glm::vec3(169., 149., 149.)*(1.5f / 255.f));
	cloudColorBottom = (glm::vec3(65., 70., 80.)*(1.5f / 255.f));

	weatheringID = 0;
	perlinID = 0;
	worleyID = 0;
}

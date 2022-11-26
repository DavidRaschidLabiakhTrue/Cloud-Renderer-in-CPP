#include "Skybox.hpp"

#include "Scene.hpp"
#include "../CoreWorkerBackend/Window.hpp"
#include "../imgui/imgui.h"

Skybox::Skybox() : timeOperator()
{
	skyColorTop = glm::vec3(0.5, 0.7, 0.8)*1.05f;
	skyColorBottom = glm::vec3(0.9, 0.9, 0.95);

	skyboxShader = new PostProcessor("shaders/sky.frag");
	skyboxFBO = new FrameBufferObject(Window::ScreenWidth, Window::ScreenHeight);

	presetSunset.cloudColorBottom = glm::vec3(95.0f, 96.0f, 1) ToScale;
	presetSunset.skyColorTop = glm::vec3(131.0f, 154.0f, 210.0f) ToScale;
	presetSunset.skyColorBottom = glm::vec3(239.0f, 158.0f, 159.5f) ToScale;
	presetSunset.lightColor = glm::vec3(242.0f, 200.0f, 201.0f) ToScale;
	presetSunset.fogColor = glm::vec3(129.05f, 153.0f, 168.0f) ToScale;
	highSunPreset.cloudColorBottom = (glm::vec3(64.0f, 71.0f, 75.0f) * (1.2f ToScale));
	highSunPreset.skyColorTop = glm::vec3(0.4, 0.7, 0.9) * 1.08f;
	highSunPreset.skyColorBottom = glm::vec3(0.85, 0.95, 0.90);
	highSunPreset.lightColor = glm::vec3(254, 253, 231) ToScale;
	highSunPreset.fogColor = glm::vec3(0.6, 0.5, 0.5);
}

void Skybox::setGui() 
{
	ImGui::Begin(" ");
	
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Time Of Day Controls");
	timeOperator.setGui();
	ImGui::End();
}




void Skybox::mergedLoadedColors(float v, PreloadedColor p1, PreloadedColor p2) 
{
	float a = std::min(std::max(v, 0.0f), 1.0f);
	float b = 1.0f - a;
	skyColorTop = p1.skyColorTop*a + p2.skyColorTop*b;
	skyColorBottom = p1.skyColorBottom*a + p2.skyColorBottom*b;
	scene->lightColor = p1.lightColor*a + p2.lightColor*b;
	scene->fogColor = p1.fogColor*a + p2.fogColor*b;
}


void Skybox::draw() 
{
	Scene * s = Drawable::scene;
	skyboxFBO->bind();
	Shader& shader = skyboxShader->getShader();
	shader.use();
	shader.uploadVector2ToGPU("resolution", glm::vec2(Window::ScreenWidth, Window::ScreenHeight));
	shader.uploadMatrix4ToGPU("inv_proj", glm::inverse(s->proj));
	shader.uploadMatrix4ToGPU("inv_view", glm::inverse(s->cam->GetViewMatrix()));
	shader.uploadVector3ToGPU("lightDirection", glm::normalize(s->lightPos - s->cam->cameraPosition));
	shader.uploadVector3ToGPU("skyColorTop", skyColorTop);
	shader.uploadVector3ToGPU("skyColorBottom", skyColorBottom);
	skyboxShader->draw();
}

void Skybox::update() 
{
	presetSunset.skyColorTop = highSunPreset.skyColorTop = timeOperator.col.upperAtmosphereColor;
	presetSunset.skyColorBottom = highSunPreset.skyColorBottom = timeOperator.col.lowerAtmosphereColor;
	mergedLoadedColors((1 / (1.0 + exp(8.0 - scene->lightDir.y * 40.0))), highSunPreset, presetSunset);
}

Skybox::~Skybox()
{
	delete skyboxFBO;
	delete skyboxShader;
}

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

	presetSunset.cloudColorBottom = glm::vec3(97, 98, 120) / 255.f;
	presetSunset.skyColorTop = glm::vec3(133, 158, 214) / 255.f;
	presetSunset.skyColorBottom = glm::vec3(241, 161, 161) / 255.f;
	presetSunset.lightColor = glm::vec3(255, 201, 201) / 255.f;
	presetSunset.fogColor = glm::vec3(128, 153, 179) / 255.f;
	highSunPreset.cloudColorBottom = (glm::vec3(65., 70., 80.) * (1.5f / 255.f));
	highSunPreset.skyColorTop = glm::vec3(0.5, 0.7, 0.8) * 1.05f;
	highSunPreset.skyColorBottom = glm::vec3(0.9, 0.9, 0.95);
	highSunPreset.lightColor = glm::vec3(255, 255, 230) / 255.f;
	highSunPreset.fogColor = glm::vec3(0.5, 0.6, 0.7);
}

void Skybox::setGui() 
{
	ImGui::Begin(" ");
	
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Time Of Day Controls");
	timeOperator.setGui();
	ImGui::End();
}




void Skybox::mixSkyColorPreset(float v, PreloadedColor p1, PreloadedColor p2) 
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
	shader.setVec2("resolution", glm::vec2(Window::ScreenWidth, Window::ScreenHeight));
	shader.setMat4("inv_proj", glm::inverse(s->proj));
	shader.setMat4("inv_view", glm::inverse(s->cam->GetViewMatrix()));
	shader.setVec3("lightDirection", glm::normalize(s->lightPos - s->cam->cameraPosition));
	shader.setVec3("skyColorTop", skyColorTop);
	shader.setVec3("skyColorBottom", skyColorBottom);
	skyboxShader->draw();
}

void Skybox::update() 
{
	presetSunset.skyColorTop = highSunPreset.skyColorTop = timeOperator.col.upperAtmosphereColor;
	presetSunset.skyColorBottom = highSunPreset.skyColorBottom = timeOperator.col.lowerAtmosphereColor;
	mixSkyColorPreset((1 / (1.0 + exp(8.0 - scene->lightDir.y * 40.0))), highSunPreset, presetSunset);
}

Skybox::~Skybox()
{
	delete skyboxFBO;
	delete skyboxShader;
}

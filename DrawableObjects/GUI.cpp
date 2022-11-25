#include "GUI.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include "../Engine/Utils.hpp"

GUI::GUI(Window& window)
{
	// GUI
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 430");
}

void GUI::draw()
{
		Scene& scene = *this->scene;

		for (Drawable* obj : subscribers) 
		{
			obj->setGui();
		}

		ImGui::Begin(" ");
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Scene");
		if (ImGui::DragFloat3("Light Position", &scene.lightDir[0], 0.01, -1.0, 1.0)) 
		{
			auto saturate = [](float v) { return std::min(std::max(v, 0.0f), 0.8f); };
			scene.lightDir.y = saturate(scene.lightDir.y);

		}
		// ImGui::InputFloat3("Camera Position", &(scene.cam->Position[0]), 7); // hiding functionality - Do not enable - David
		// ImGui::ColorEdit3("Light color", (float*)&scene.lightColor); // hiding functionality - David


		//ImGui::ColorEdit3("Fog color", (float*)&scene.fogColor);
		ImGui::SliderFloat("Camera speed", &scene.cam->MovementSpeed, 0.0, SPEED*3.0);




		// ImGui::Checkbox("Wireframe mode", &scene.wireframe);

		if (ImGui::Button("Generate seed"))
		{

			std::random_device rd;  
			std::mt19937 gen(rd()); 
			std::uniform_real_distribution<> dis(.0, 100.);
			scene.seed = vec3(dis(gen), dis(gen), dis(gen));
		}
			
		//ImGui::SameLine();
		//ImGui::Text("Generate a new seed");
		ImGui::SameLine();
		if (ImGui::Button("Use default seed"))
			scene.seed = glm::vec3(0.0, 0.0, 0.0);

		/*ImGui::SameLine();
		if (ImGui::Button("Default Preset")) {
			volumetricClouds.DefaultPreset();
			lightDir.y = 0.5;
		}*/
		//ImGui::SameLine();
		/*if (ImGui::Button("Sunset Preset 1")) {
			skybox.SunsetPreset();
		}
		ImGui::SameLine();
		if (ImGui::Button("Sunset Preset 2")) {
			skybox.SunsetPreset1();
		}*/

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	

	//actual drawing
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::update()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

GUI & GUI::subscribe(Drawable * subscriber)
{
	subscribers.push_back(subscriber);

	return *this;
}


GUI::~GUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

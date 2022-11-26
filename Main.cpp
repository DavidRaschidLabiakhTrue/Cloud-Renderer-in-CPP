#include "Main.hpp"

#include <random>
#include <time.h>     

using glm::vec2;

Main::Main() : camera(vec3(0.0f, 800.0f, 0.0f)) , window(), gui(window), PostProcessing("shaders/post_processing.frag")
{
	window.camera = &camera; //Window class needs camera address to perform input handling
}

Main::~Main()
{
}

int Main::run()
{
	if (window.successfulLoad == false)
	{
		return -1; // failure
	}

	vec3 fogColor = vec3(0.5, 0.6, 0.7);
	vec3 lightColor = vec3(255.0f, 255.0f, 230.0f) / 255.0f;

	FrameBufferObject SceneFBO(Window::ScreenWidth, Window::ScreenHeight);
	vec3 lightPosition;

	srand(time(NULL)); // seed random time;
	std::mt19937 generator(rand()); // this ensures it's random every single time like a cloud should be. A cloud should never be reproducible
	std::uniform_real_distribution<float> dis(0.0, 1.0); // normal distribution
	vec3 seed = vec3(dis(generator) + rand(), dis(generator) + rand(), dis(generator) + rand());

	proj = glm::perspective(glm::radians(camera.cameraZoom), (float)Window::ScreenWidth / (float)Window::ScreenHeight, 5.f, 10000000.0f);
	vec3 lightDir = vec3(-0.5f, 0.5f, 1.0f);


	Scene scene;
	scene.loadLight(lightPosition, lightColor, lightDir).loadAttributes(fogColor, seed).loadCameraData(camera, proj).loadFrameBuffer(SceneFBO);




	Drawable::scene = &scene; // load reference of scene onto it's static pointer in drawables
	// this lets all things that inherit drawable access the scene with ease.

	// The alternative is globals. Avoiding where I can.

	int planeGridLength = 150;
	ReflectablePlane reflectablePlane(150);
	PlaneCounter planeCounter(vec2(0.0, 0.0), planeGridLength, 0);
	Skybox skybox;
	reflectablePlane.setSkybox(skybox);
	DrawableClouds cloudsModel(&scene, &skybox);
	VolumetricClouds volumetricClouds(Window::ScreenWidth, Window::ScreenHeight, &cloudsModel);
	VolumetricClouds reflectionVolumetricClouds(1280, 720, &cloudsModel); // (expected) lower resolution framebuffers, so the rendering will be faster
	gui.attach(&cloudsModel).attach(&skybox);

	float frametime = 0.0f;
	while (window.continueLoop())
	{
		scene.lightDir = glm::normalize(scene.lightDir);
		scene.lightPos = scene.lightDir * 1e6f + camera.cameraPosition;
		frametime = 1 / ImGui::GetIO().Framerate;
		window.processInput(frametime);
		cloudsModel.update(); // update clouds
		gui.update();
		skybox.update(); // update skybox
		SceneFBO.bind();
		clear1();
		view = scene.cam->GetViewMatrix();
		scene.proj = glm::perspective(glm::radians(camera.cameraZoom), (float)Window::ScreenWidth / (float)Window::ScreenHeight, 5.f, 10000000.0f);
		planeCounter.bindReflectionFBO(); // dummy code
		clear2();
		scene.updateCameraAttributes(planeCounter.getHeight());// dummy code
		reflectablePlane.setUp(1.0f).draw();
		FrameBufferObject const& reflFBO = planeCounter.getReflectionFBO();// dummy code
		PostProcessor::disableTests();
		reflectionVolumetricClouds.draw();
		planeCounter.bindReflectionFBO(); // dummy code
		postProcess1(reflFBO, reflectionVolumetricClouds);
		scene.updateCameraAttributes(planeCounter.getHeight());// dummy code
		planeCounter.bindRefractionFBO();
		clear3();
		reflectablePlane.setUp(-1.0f).draw();// dummy code
		scene.sceneFBO->bind();
		reflectablePlane.draw();// dummy code
		planeCounter.draw();// dummy code
		PostProcessor::disableTests();
		volumetricClouds.draw();
		skybox.draw();
		postProcess2(scene, SceneFBO, volumetricClouds); // Smooth out
		gui.draw();
		window.swapBuffersAndPollEvents();
	}
	return 0;
}



void Main::postProcess1(const FrameBufferObject& fbo, VolumetricClouds& volumetricClouds)
{
	Shader& post = PostProcessing.getShader();
	post.use();
	post.setVec2("resolution", vec2(1280, 720));
	post.setSampler2D("screenTexture", fbo.textureID, 0);
	post.setSampler2D("depthTex", fbo.depthTextureID, 2);
	post.setSampler2D("cloudTEX", volumetricClouds.getCloudsRawTexture(), 1);
	PostProcessing.draw();
	PostProcessor::enableTests();
}

void Main::postProcess2(const Scene& scene, const FrameBufferObject& fbo, VolumetricClouds& volumetricClouds)
{
	unbindCurrentFrameBuffer();
	Shader& post = PostProcessing.getShader();
	post.use();
	post.setVec2("resolution", vec2(Window::ScreenWidth, Window::ScreenHeight));
	post.setVec3("cameraPosition", scene.cam->cameraPosition);
	post.setSampler2D("screenTexture", fbo.textureID, 0);
	post.setSampler2D("cloudTEX", volumetricClouds.getCloudsTexture(), 1);
	post.setSampler2D("depthTex", fbo.depthTextureID, 2);
	post.setSampler2D("cloudDistance", volumetricClouds.getCloudsTexture(VolumetricClouds::cloudDistance), 3);
	post.setMat4("VP", scene.proj * view);
	PostProcessing.draw();
}

void Main::clear1()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(.3, .3, .3, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Main::clear2()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Main::clear3()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

int main()
{
	Main app;

	return app.run();
	
}



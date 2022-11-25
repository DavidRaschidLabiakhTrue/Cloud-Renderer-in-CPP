#include "Main.hpp"

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

	glm::vec3 fogColor(0.5, 0.6, 0.7);
	glm::vec3 lightColor(255, 255, 230);
	lightColor /= 255.0;

	FrameBufferObject SceneFBO(Window::ScreenWidth, Window::ScreenHeight);
	glm::vec3 lightPosition, seed;
	proj = glm::perspective(glm::radians(camera.Zoom), (float)Window::ScreenWidth / (float)Window::ScreenHeight, 5.f, 10000000.0f);
	glm::vec3 lightDir = glm::vec3(-.5, 0.5, 1.0);

	//Every scene object need these informations to be rendered
	Scene scene;

	scene.loadLight(lightPosition, lightColor, lightDir).loadAttributes(fogColor, seed).loadCameraData(camera, proj).loadFrameBuffer(SceneFBO);




	Drawable::scene = &scene; // load static reference

	int gridLength = 150;
	ReflectablePlane reflectablePlane(150);


	PlaneCounter planeCounter(glm::vec2(0.0, 0.0), gridLength, 0);


	Skybox skybox;

	reflectablePlane.setSkybox(skybox);
	DrawableClouds cloudsModel(&scene, &skybox);

	VolumetricClouds volumetricClouds(Window::ScreenWidth, Window::ScreenHeight, &cloudsModel);
	VolumetricClouds reflectionVolumetricClouds(1280, 720, &cloudsModel); // (expected) lower resolution framebuffers, so the rendering will be faster

	gui.attach(&cloudsModel).attach(&skybox);


	while (window.continueLoop())
	{
		scene.lightDir = glm::normalize(scene.lightDir);
		scene.lightPos = scene.lightDir * 1e6f + camera.Position;

		// input
		float frametime = 1 / ImGui::GetIO().Framerate;
		window.processInput(frametime);

		//update tiles position to make the world infinite, clouds weather map and sky colors
		//reflectablePlane.updateTilesPositions();


		cloudsModel.update(); // update clouds
		gui.update();
		skybox.update(); // update skybox

		SceneFBO.bind();

		clear1();



		view = scene.cam->GetViewMatrix();
		scene.proj = glm::perspective(glm::radians(camera.Zoom), (float)Window::ScreenWidth / (float)Window::ScreenHeight, 5.f, 10000000.0f);



		planeCounter.bindReflectionFBO();
		clear2();

		scene.updateCameraAttributes(planeCounter.getHeight());


		reflectablePlane.up = 1.0;
		reflectablePlane.draw();
		FrameBufferObject const& reflFBO = planeCounter.getReflectionFBO();

		PostProcessor::disableTests();

		reflectionVolumetricClouds.draw();
		planeCounter.bindReflectionFBO(); //rebind refl buffer; reflVolumetricClouds unbound it

		postProcess1(reflFBO, reflectionVolumetricClouds);


		

		scene.cam->invertPitch();
		scene.cam->Position.y += 2 * abs(scene.cam->Position.y - planeCounter.getHeight());

		//draw to water refraction buffer object
		planeCounter.bindRefractionFBO();

		clear3();

		reflectablePlane.up = -1.0;
		reflectablePlane.draw();

		
		scene.sceneFBO->bind();
		reflectablePlane.draw();
		planeCounter.draw();

		
		PostProcessor::disableTests();

		volumetricClouds.draw();
		skybox.draw();

		postProcess2(scene, SceneFBO, volumetricClouds);
		gui.draw();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		window.swapBuffersAndPollEvents();
	}
	return 0;
}



void Main::postProcess1(const FrameBufferObject& fbo, VolumetricClouds& volumetricClouds)
{
	Shader& post = PostProcessing.getShader();
	post.use();
	post.setVec2("resolution", glm::vec2(1280, 720));
	post.setSampler2D("screenTexture", fbo.tex, 0);
	post.setSampler2D("depthTex", fbo.depthTex, 2);
	post.setSampler2D("cloudTEX", volumetricClouds.getCloudsRawTexture(), 1);
	PostProcessing.draw();
	PostProcessor::enableTests();
}

void Main::postProcess2(const Scene& scene, const FrameBufferObject& fbo, VolumetricClouds& volumetricClouds)
{
	unbindCurrentFrameBuffer();
	Shader& post = PostProcessing.getShader();
	post.use();
	post.setVec2("resolution", glm::vec2(Window::ScreenWidth, Window::ScreenHeight));
	post.setVec3("cameraPosition", scene.cam->Position);
	post.setSampler2D("screenTexture", fbo.tex, 0);
	post.setSampler2D("cloudTEX", volumetricClouds.getCloudsTexture(), 1);
	post.setSampler2D("depthTex", fbo.depthTex, 2);
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



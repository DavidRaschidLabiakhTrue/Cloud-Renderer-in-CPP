#include "VolumetricClouds.hpp"

VolumetricClouds::VolumetricClouds(int SW, int SH, DrawableClouds * model): SCR_WIDTH(SW), SCR_HEIGHT(SH), model(model) {

	cloudsFBO = new TextureSet(SW, SH, 4);
	cloudsPostProcessingFBO = new FrameBufferObject(Window::ScreenWidth, Window::ScreenHeight, 2);

}


void VolumetricClouds::draw() 
{

	float t1, t2;

	const auto nTextures = cloudsFBO->getNTextures();
	for (int i = 0; i < nTextures; ++i)
	{
		bindTexture2D(cloudsFBO->getColorAttachmentTex(i), i);
	}

	Shader & cloudData = *model->volumetricCloudsShader;
	Scene* s = Drawable::scene;

	cloudData.use();

	cloudData.setVec2("iResolution", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
	cloudData.setFloat("iTime", glfwGetTime());
	cloudData.setMat4("inv_proj", glm::inverse(s->projMatrix));
	cloudData.setMat4("inv_view", glm::inverse(s->cam->GetViewMatrix()));
	cloudData.setVec3("cameraPosition", s->cam->Position);
	cloudData.setFloat("FOV", s->cam->Zoom);
	cloudData.setVec3("lightDirection", glm::normalize(s->lightPos - s->cam->Position));
	cloudData.setVec3("lightColor", s->lightColor);
	
	cloudData.setFloat("coverage_multiplier", model->coverage);
	cloudData.setFloat("cloudSpeed", model->cloudSpeed);
	cloudData.setFloat("crispiness", model->crispiness);
	cloudData.setFloat("curliness", model->curliness);
	cloudData.setFloat("absorption", model->absorption*0.01);
	cloudData.setFloat("densityFactor", model->density);

	//cloudsShader.setBool("enablePowder", enablePowder);
	
	cloudData.setFloat("earthRadius", model->earthRadius);
	cloudData.setFloat("sphereInnerRadius", model->sphereInnerRadius);
	cloudData.setFloat("sphereOuterRadius", model->sphereOuterRadius);

	cloudData.setVec3("cloudColorTop", model->cloudColorTop);
	cloudData.setVec3("cloudColorBottom", model->cloudColorBottom);
	
	cloudData.setVec3("skyColorTop", model->sky->skyColorTop);
	cloudData.setVec3("skyColorBottom", model->sky->skyColorBottom);

	glm::mat4 vp = s->projMatrix*s->cam->GetViewMatrix();
	cloudData.setMat4("invViewProj", glm::inverse(vp));
	cloudData.setMat4("gVP", vp);

	cloudData.setSampler3D("cloud", model->perlinID, 0);
	cloudData.setSampler3D("worley32", model->worleyID, 1);
	cloudData.setSampler2D("weatherTex", model->weatheringID, 2);
	cloudData.setSampler2D("depthMap", s->sceneFBO->depthTex, 3);

	cloudData.setSampler2D("sky", model->sky->getSkyTexture(), 4);


	if(!s->wireframe)
		glDispatchCompute(INT_CEIL(SCR_WIDTH, 16), INT_CEIL(SCR_HEIGHT, 16), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	
	//copy to lastFrameFBO

	if (model->postProcess) 
	{
		// cloud post processing filtering
		cloudsPostProcessingFBO->bind();
		Shader& cloudsPPShader = model->postProcessingShader->getShader();

		cloudsPPShader.use();

		cloudsPPShader.setSampler2D("clouds", cloudsFBO->getColorAttachmentTex(VolumetricClouds::fragColor), 0);
		cloudsPPShader.setSampler2D("emissions", cloudsFBO->getColorAttachmentTex(VolumetricClouds::bloom), 1);
		cloudsPPShader.setSampler2D("depthMap", s->sceneFBO->depthTex, 2);

		cloudsPPShader.setVec2("cloudRenderResolution", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
		cloudsPPShader.setVec2("resolution", glm::vec2(Window::ScreenWidth , Window::ScreenHeight));

		glm::mat4 lightModel;
		lightModel = glm::translate(lightModel, s->lightPos);
		glm::vec4 pos = vp * lightModel * glm::vec4(0.0, 60.0, 0.0, 1.0);
		pos = pos / pos.w;
		pos = pos * 0.5f + 0.5f;

		//std::cout << pos.x << ": X; " << pos.y << " Y;" << std::endl;
		cloudsPPShader.setVec4("lightPos", pos);

		bool isLightInFront = false;
		float lightDotCameraFront = glm::dot(glm::normalize(s->lightPos - s->cam->Position), glm::normalize(s->cam->Front));
		//std::cout << "light dot camera front= " << lightDotCameraFront << std::endl;
		if (lightDotCameraFront > 0.2) {
			isLightInFront = true;
		}

		cloudsPPShader.setBool("isLightInFront", isLightInFront);
		cloudsPPShader.setBool("enableGodRays", model->enableGodRays);
		cloudsPPShader.setFloat("lightDotCameraFront", lightDotCameraFront);

		cloudsPPShader.setFloat("time", glfwGetTime());
		if (!s->wireframe)
			model->postProcessingShader->draw();
	}
}



VolumetricClouds::~VolumetricClouds()
{
}

#include "VolumetricClouds.hpp"

VolumetricClouds::VolumetricClouds(int SW, int SH, DrawableClouds * drawableCloudModels): screenWidth(SW), screenHeight(SH), drawableCloudModels(drawableCloudModels) {

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

	Shader & cloudData = *drawableCloudModels->volumetricCloudsShader;
	Scene* s = Drawable::scene;

	cloudData.use();
	cloudData.setVec2("iResolution", glm::vec2(screenWidth, screenHeight));
	cloudData.setFloat("iTime", glfwGetTime());
	cloudData.setMat4("inv_proj", glm::inverse(s->proj));
	cloudData.setMat4("inv_view", glm::inverse(s->cam->GetViewMatrix()));
	cloudData.setVec3("cameraPosition", s->cam->cameraPosition);
	cloudData.setFloat("FOV", s->cam->cameraZoom);
	cloudData.setVec3("lightDirection", glm::normalize(s->lightPos - s->cam->cameraPosition));
	cloudData.setVec3("lightColor", s->lightColor);
	cloudData.setFloat("coverage_multiplier", drawableCloudModels->cloudCoverage);
	cloudData.setFloat("cloudSpeed", drawableCloudModels->cloudSpeed);
	cloudData.setFloat("crispiness", drawableCloudModels->cloudCrispiness);
	cloudData.setFloat("curliness", drawableCloudModels->cloudCurliness);
	cloudData.setFloat("absorption", drawableCloudModels->cloudAbsorption*0.01);
	cloudData.setFloat("densityFactor", drawableCloudModels->cloudDensity);
	cloudData.setFloat("earthRadius", drawableCloudModels->spatialRadiusOfRenderSpace);
	cloudData.setFloat("sphereInnerRadius", drawableCloudModels->sphereInnerRadius);
	cloudData.setFloat("sphereOuterRadius", drawableCloudModels->sphereOuterRadius);
	cloudData.setVec3("cloudColorTop", drawableCloudModels->cloudColorTop);
	cloudData.setVec3("cloudColorBottom", drawableCloudModels->cloudColorBottom);
	cloudData.setVec3("skyColorTop", drawableCloudModels->sky->skyColorTop);
	cloudData.setVec3("skyColorBottom", drawableCloudModels->sky->skyColorBottom);
	glm::mat4 vp = s->proj*s->cam->GetViewMatrix();
	cloudData.setMat4("invViewProj", glm::inverse(vp));
	cloudData.setMat4("gVP", vp);

	cloudData.setSampler3D("cloud", drawableCloudModels->perlinID, 0);
	cloudData.setSampler3D("worley32", drawableCloudModels->worleyID, 1);
	cloudData.setSampler2D("weatherTex", drawableCloudModels->weatheringID, 2);
	cloudData.setSampler2D("depthMap", s->sceneFBO->depthTextureID, 3);

	cloudData.setSampler2D("sky", drawableCloudModels->sky->getSkyTexture(), 4);


	if (!s->wireframe)
	{
		glDispatchCompute(INT_CEIL(screenWidth, 16), INT_CEIL(screenHeight, 16), 1);
	}
		
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	


	if (drawableCloudModels->postProcess) 
	{

		cloudsPostProcessingFBO->bind();
		Shader& cloudsPPShader = drawableCloudModels->postProcessingShader->getShader();

		cloudsPPShader.use();

		cloudsPPShader.setSampler2D("clouds", cloudsFBO->getColorAttachmentTex(VolumetricClouds::cloudFragColor), 0);
		cloudsPPShader.setSampler2D("emissions", cloudsFBO->getColorAttachmentTex(VolumetricClouds::cloudBloom), 1);
		cloudsPPShader.setSampler2D("depthMap", s->sceneFBO->depthTextureID, 2);

		cloudsPPShader.setVec2("cloudRenderResolution", glm::vec2(screenWidth, screenHeight));
		cloudsPPShader.setVec2("resolution", glm::vec2(Window::ScreenWidth , Window::ScreenHeight));

		glm::mat4 lightModel;
		lightModel = glm::translate(lightModel, s->lightPos);
		glm::vec4 pos = vp * lightModel * glm::vec4(0.0, 60.0, 0.0, 1.0);
		pos = pos / pos.w;
		pos = pos * 0.5f + 0.5f;
		cloudsPPShader.setVec4("lightPos", pos);
		bool isLightInFront = false;
		float lightDotCameraFront = glm::dot(glm::normalize(s->lightPos - s->cam->cameraPosition), glm::normalize(s->cam->cameraFront));
		if (lightDotCameraFront > 0.2) 
		{
			isLightInFront = true;
		}
		cloudsPPShader.setBool("isLightInFront", isLightInFront);
		cloudsPPShader.setFloat("lightDotCameraFront", lightDotCameraFront);
		cloudsPPShader.setFloat("time", glfwGetTime());
		if (!s->wireframe)
		{
			drawableCloudModels->postProcessingShader->draw();
		}
			
	}
}



VolumetricClouds::~VolumetricClouds()
{
}

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
	cloudData.uploadVector2ToGPU("iResolution", glm::vec2(screenWidth, screenHeight));
	cloudData.uploadFloatToGPU("iTime", glfwGetTime());
	cloudData.uploadMatrix4ToGPU("inv_proj", glm::inverse(s->proj));
	cloudData.uploadMatrix4ToGPU("inv_view", glm::inverse(s->cam->GetViewMatrix()));
	cloudData.uploadVector3ToGPU("cameraPosition", s->cam->cameraPosition);
	cloudData.uploadFloatToGPU("FOV", s->cam->cameraZoom);
	cloudData.uploadVector3ToGPU("lightDirection", glm::normalize(s->lightPos - s->cam->cameraPosition));
	cloudData.uploadVector3ToGPU("lightColor", s->lightColor);
	cloudData.uploadFloatToGPU("coverage_multiplier", drawableCloudModels->cloudCoverage);
	cloudData.uploadFloatToGPU("cloudSpeed", drawableCloudModels->cloudSpeed);
	cloudData.uploadFloatToGPU("crispiness", drawableCloudModels->cloudCrispiness);
	cloudData.uploadFloatToGPU("curliness", drawableCloudModels->cloudCurliness);
	cloudData.uploadFloatToGPU("absorption", drawableCloudModels->cloudAbsorption*0.01);
	cloudData.uploadFloatToGPU("densityFactor", drawableCloudModels->cloudDensity);
	cloudData.uploadFloatToGPU("earthRadius", drawableCloudModels->spatialRadiusOfRenderSpace);
	cloudData.uploadFloatToGPU("sphereInnerRadius", drawableCloudModels->sphereInnerRadius);
	cloudData.uploadFloatToGPU("sphereOuterRadius", drawableCloudModels->sphereOuterRadius);
	cloudData.uploadVector3ToGPU("cloudColorTop", drawableCloudModels->cloudColorTop);
	cloudData.uploadVector3ToGPU("cloudColorBottom", drawableCloudModels->cloudColorBottom);
	cloudData.uploadVector3ToGPU("skyColorTop", drawableCloudModels->sky->skyColorTop);
	cloudData.uploadVector3ToGPU("skyColorBottom", drawableCloudModels->sky->skyColorBottom);
	glm::mat4 vp = s->proj*s->cam->GetViewMatrix();
	cloudData.uploadMatrix4ToGPU("invViewProj", glm::inverse(vp));
	cloudData.uploadMatrix4ToGPU("gVP", vp);

	cloudData.uploadSampler3DToGPU("cloud", drawableCloudModels->perlinID, 0);
	cloudData.uploadSampler3DToGPU("worley32", drawableCloudModels->worleyID, 1);
	cloudData.uploadSampler2DToGPU("weatherTex", drawableCloudModels->weatheringID, 2);
	cloudData.uploadSampler2DToGPU("depthMap", s->sceneFBO->depthTextureID, 3);

	cloudData.uploadSampler2DToGPU("sky", drawableCloudModels->sky->getSkyTexture(), 4);


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

		cloudsPPShader.uploadSampler2DToGPU("clouds", cloudsFBO->getColorAttachmentTex(VolumetricClouds::cloudFragColor), 0);
		cloudsPPShader.uploadSampler2DToGPU("emissions", cloudsFBO->getColorAttachmentTex(VolumetricClouds::cloudBloom), 1);
		cloudsPPShader.uploadSampler2DToGPU("depthMap", s->sceneFBO->depthTextureID, 2);

		cloudsPPShader.uploadVector2ToGPU("cloudRenderResolution", glm::vec2(screenWidth, screenHeight));
		cloudsPPShader.uploadVector2ToGPU("resolution", glm::vec2(Window::ScreenWidth , Window::ScreenHeight));

		glm::mat4 lightModel;
		lightModel = glm::translate(lightModel, s->lightPos);
		glm::vec4 pos = vp * lightModel * glm::vec4(0.0, 60.0, 0.0, 1.0);
		pos = pos / pos.w;
		pos = pos * 0.5f + 0.5f;
		cloudsPPShader.uploadVector4ToGPU("lightPos", pos);
		bool isLightInFront = false;
		float lightDotCameraFront = glm::dot(glm::normalize(s->lightPos - s->cam->cameraPosition), glm::normalize(s->cam->cameraFront));
		if (lightDotCameraFront > 0.2) 
		{
			isLightInFront = true;
		}
		cloudsPPShader.uploadBoolToGPU("isLightInFront", isLightInFront);
		cloudsPPShader.uploadFloatToGPU("lightDotCameraFront", lightDotCameraFront);
		cloudsPPShader.uploadFloatToGPU("time", glfwGetTime());
		if (!s->wireframe)
		{
			drawableCloudModels->postProcessingShader->draw();
		}
			
	}
}



VolumetricClouds::~VolumetricClouds()
{
}

#pragma once
#include "../CoreWorkerBackend/PostProcess.hpp"
#include "../CoreWorkerBackend/buffers.hpp"
#include "../CoreWorkerBackend/Texture.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Camera.hpp>
#include <algorithm>
#include "Drawable.hpp"
#include "DrawableClouds.hpp"


class VolumetricClouds : public Drawable
{
	public:
		VolumetricClouds(int SW, int SH, DrawableClouds * drawableCloudModels);
		virtual void draw();
		~VolumetricClouds();

		enum cloudsTextureNames 
		{
			cloudFragColor, 
			cloudBloom, 
			cloudAlphaness, 
			cloudDistance
		};

		unsigned int getCloudsTexture() 
		{ 
			return (drawableCloudModels->postProcess ? cloudsPostProcessingFBO->getColorAttachmentTex(0) : getCloudsRawTexture());
		}

		unsigned int getCloudsTexture(int i) 
		{
			return cloudsFBO->getColorAttachmentTex(i);
		}

		unsigned int getCloudsRawTexture()
		{
			return cloudsFBO->getColorAttachmentTex(0);
		}

		int screenWidth, screenHeight;
		TextureSet * cloudsFBO;
		FrameBufferObject *cloudsPostProcessingFBO;
		DrawableClouds * drawableCloudModels;
};


#pragma once
#include <glad/glad.h>
#include "Window.hpp"



void unbindCurrentFrameBuffer(int scrWidth, int scrHeight);
void unbindCurrentFrameBuffer();

unsigned int createFrameBuffer();

unsigned int createTextureAttachment(int width, int height);

unsigned int * createColorAttachments(int width, int height, unsigned int nColorAttachments);

unsigned int createDepthTextureAttachment(int width, int height);




class FrameBufferObject 
{
	public:
		FrameBufferObject(int frameWidth, int frameHeight);
		FrameBufferObject(int frameWidth, int frameHeight, int numColorAttachments);
		unsigned int fbo;
		unsigned int renderBufferID;
		unsigned int depthTextureID;
		unsigned int textureID;
		unsigned int getColorAttachmentTex(int i);
		void bind();
	private:
		int frameWidth;
		int frameHeight;
		int nColorAttachments;
		unsigned int * colorAttachments;
};

class TextureSet 
{
	public:
		int nTextures;
		unsigned int* texturePointer;
		TextureSet(int frameWidth, int frameHeight, int num);
		void bindTexture(int i, int unit);
		unsigned int getColorAttachmentTex(int i);
		void bind();
		int getNTextures() const
		{
			return nTextures;
		}
};
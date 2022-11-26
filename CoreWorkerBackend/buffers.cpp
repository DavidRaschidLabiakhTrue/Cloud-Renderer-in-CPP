#include "buffers.hpp"
#include "Texture.hpp"


void unbindCurrentFrameBuffer(int scrWidth, int scrHeight) 
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, scrWidth, scrHeight);
}

void unbindCurrentFrameBuffer() 
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::ScreenWidth, Window::ScreenHeight);
}

unsigned int createFrameBuffer() 
{
	unsigned int frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	return frameBuffer;
}

unsigned int createTextureAttachment(int width, int height) 
{
	unsigned int texturePointer;
	glGenTextures(1, &texturePointer);
	glBindTexture(GL_TEXTURE_2D, texturePointer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texturePointer, 0);

	return texturePointer;
}

unsigned int * createColorAttachments(int width, int height, unsigned int nColorAttachments) 
{
	unsigned int * colorAttachments = new unsigned int[nColorAttachments];
	glGenTextures(nColorAttachments, colorAttachments);
	for (unsigned int i = 0; i < nColorAttachments; i++) 
	{
		glBindTexture(GL_TEXTURE_2D, colorAttachments[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorAttachments[i], 0);
	}
	return colorAttachments;
}

unsigned int createDepthTextureAttachment(int width, int height) 
{
	unsigned int texturePointer;
	glGenTextures(1, &texturePointer);
	glBindTexture(GL_TEXTURE_2D, texturePointer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texturePointer, 0);
	return texturePointer;
}





FrameBufferObject::FrameBufferObject(int W_, int H_) 
{
	this->frameWidth = W_;
	this->frameHeight = H_;
	this->fbo = createFrameBuffer();
	this->textureID = createTextureAttachment(frameWidth, frameHeight);
	this->depthTextureID = createDepthTextureAttachment(frameWidth, frameHeight);

	colorAttachments = NULL;
	nColorAttachments = 1;
}

FrameBufferObject::FrameBufferObject(int W_, int H_, const int nColorAttachments) 
{
	this->frameWidth = W_;
	this->frameHeight = H_;
	this->fbo = createFrameBuffer();

	this->textureID = NULL;
	this->depthTextureID = createDepthTextureAttachment(frameWidth, frameHeight);
	this->colorAttachments = createColorAttachments(frameWidth, frameHeight, nColorAttachments);
	this->nColorAttachments = nColorAttachments;

	unsigned int * colorAttachmentsFlag = new unsigned int[nColorAttachments];
	for (unsigned int i = 0; i < nColorAttachments; i++) 
	{
		colorAttachmentsFlag[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	glDrawBuffers(nColorAttachments, colorAttachmentsFlag);
	delete colorAttachmentsFlag;
}


void FrameBufferObject::bind() 
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
	glViewport(0, 0, this->frameWidth, this->frameHeight);
}

unsigned int FrameBufferObject::getColorAttachmentTex(int i) 
{
	if (i < 0 || i > nColorAttachments) 
	{
		std::cout << "COLOR ATTACHMENT OUT OF RANGE" << std::endl;
		return 0;
	}
	return colorAttachments[i];
}

TextureSet::TextureSet(int frameWidth, int frameHeight, int num)
{
	if (frameWidth > 0 && frameHeight > 0 && num > 0) 
	{
		nTextures = num;
		texturePointer = new unsigned int[num];
		for (int i = 0; i < num; ++i) 
		{
			texturePointer[i] = generateTexture2D(frameWidth, frameHeight);
		}
	}
}


unsigned int TextureSet::getColorAttachmentTex(int i) 
{
	if (i < 0 || i > nTextures) 
	{
		std::cout << "COLOR ATTACHMENT OUT OF RANGE" << std::endl;
		return 0;
	}
	return texturePointer[i];
}

void TextureSet::bindTexture(int i, int unit)
{
	bindTexture2D(texturePointer[i], unit);
}

void TextureSet::bind()
{
	for (int i = 0; i < nTextures; ++i)
	{
		bindTexture2D(texturePointer[i], i);
	}
		
}

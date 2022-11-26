#include "PostProcess.hpp"

unsigned int PostProcessor::vao = 0;
unsigned int PostProcessor::vbo = 0;
bool PostProcessor::postProcessorInitialized = false;

PostProcessor::PostProcessor(const char * fragmentPath)
{
	initialize();
	shader = new Shader("ScreenQuad_" + getShaderName(fragmentPath));
	shader->attachShader(BaseShader("shaders/screen.vert"))->attachShader(BaseShader(fragmentPath))->linkPrograms();
}

void PostProcessor::drawQuad() {
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void PostProcessor::draw() {
	PostProcessor::drawQuad();
}

PostProcessor::~PostProcessor()
{
}

void PostProcessor::initialize() {
	if (!postProcessorInitialized) 
	{
		// set of vertices that are equal to a quad with UV coordinates.
		float vertices[] = 
		{
			-1.0f, -1.0f, 0.0, 0.0,
			1.0f, -1.0f, 1.0, 0.0,
			-1.0f,  1.0f, 0.0, 1.0,
			1.0f,  1.0f, 1.0, 1.0,
			-1.0f,  1.0f, 0.0, 1.0,
			1.0f, -1.0f, 1.0, 0.0
		};


		// standard binding
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// enable position
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// enable UV
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// lock off ever running this again unless a refresh is absolutely needed
		PostProcessor::postProcessorInitialized = true;
	}

}

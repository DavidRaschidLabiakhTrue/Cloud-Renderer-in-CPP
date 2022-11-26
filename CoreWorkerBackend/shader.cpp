#include "Shader.hpp"

Shader::Shader(std::string name) : shaderName(name)
{
	isProgramLinked = false;
	isComputeShader = false;
	id = glCreateProgram();
}

Shader::~Shader() {
	glDeleteProgram(id);
}

Shader::Shader(std::string name, const char * computeShaderPath) : shaderName(name)
{
	isProgramLinked = false;
	isComputeShader = false;
	id = glCreateProgram();

	this->attachShader(CoreShaderBasis(computeShaderPath));
	this->linkPrograms();
}


Shader * Shader::attachShader(CoreShaderBasis s)
{
	if (!isComputeShader) {
		glAttachShader(id, s.getShad());
		if (s.getName() == "COMPUTE")
			isComputeShader = true;
		this->shadersToLink.push_back(s.getShad());
	}
	else {
		std::cout << "ERROR: TRYING TO LINK A NON COMPUTE SHADER TO COMPUTE PROGRAM" << std::endl;
	}

	return this;
}

void Shader::linkPrograms()
{
	glLinkProgram(id);

	if (checkCompileErrors(id, "PROGRAM", "")) 
	{
		isProgramLinked = true;
		while (!shadersToLink.empty()) 
		{
			glDeleteShader(shadersToLink.back());
			shadersToLink.pop_back();
		}
	}
	else 
	{
		std::cout << "ERROR: " << shaderName << std::endl;
	}
}


void Shader::use()
{
	if (isProgramLinked)
		glUseProgram(id);
	else {
		std::cout << "PROGRAMS NOT LINKED!" << std::endl;
	}
}

void Shader::uploadBoolToGPU(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::uploadIntToGPU(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::uploadFloatToGPU(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
void Shader::uploadVector2ToGPU(const std::string &name, glm::vec2 vector) const
{
	glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(vector));
}
void Shader::uploadVector3ToGPU(const std::string &name, glm::vec3 vector) const
{
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(vector));
}

void Shader::uploadVector4ToGPU(const std::string &name, glm::vec4 vector) const
{
	glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(vector));
}

void Shader::uploadMatrix4ToGPU(const std::string &name, glm::mat4 matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, false, glm::value_ptr(matrix));
}

void Shader::uploadSampler2DToGPU(const std::string &name, unsigned int texturePointer, int id) const
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_2D, texturePointer);
	this->uploadIntToGPU(name, id);
}
void Shader::uploadSampler3DToGPU(const std::string &name, unsigned int texturePointer, int id) const
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_3D, texturePointer);
	this->uploadIntToGPU(name, id);
}

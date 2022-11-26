#include "Shader.hpp"

Shader::Shader(std::string name) : shaderName(name)
{
	isLinked = false;
	isComputeShader = false;
	id = glCreateProgram();
}

Shader::~Shader() {
	glDeleteProgram(id);
}

Shader::Shader(std::string name, const char * computeShaderPath) : shaderName(name)
{
	isLinked = false;
	isComputeShader = false;
	id = glCreateProgram();

	this->attachShader(BaseShader(computeShaderPath));
	this->linkPrograms();
}


Shader * Shader::attachShader(BaseShader s)
{
	if (!isComputeShader) {
		glAttachShader(id, s.getShad());
		if (s.getName() == "COMPUTE")
			isComputeShader = true;
		this->shaders.push_back(s.getShad());
	}
	else {
		std::cout << "ERROR: TRYING TO LINK A NON COMPUTE SHADER TO COMPUTE PROGRAM" << std::endl;
	}

	return this;
}

void Shader::linkPrograms()
{
	glLinkProgram(id);

	if (checkCompileErrors(id, "PROGRAM", "")) {
		isLinked = true;
		std::cout << "PROGRAM " << shaderName << " CORRECTLY LINKED" << std::endl;
		while (!shaders.empty()) {
			glDeleteShader(shaders.back());
			shaders.pop_back();
		}
	}
	else {
		std::cout << "ERROR WHILE LINKING TO " << shaderName << " PROGRAM" << std::endl;
	}
}


void Shader::use()
{
	if (isLinked)
		glUseProgram(id);
	else {
		std::cout << "PROGRAMS NOT LINKED!" << std::endl;
	}
}

void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
void Shader::setVec2(const std::string &name, glm::vec2 vector) const
{
	unsigned int location = glGetUniformLocation(id, name.c_str());
	glUniform2fv(location, 1, glm::value_ptr(vector));
}
void Shader::setVec3(const std::string &name, glm::vec3 vector) const
{
	unsigned int location = glGetUniformLocation(id, name.c_str());
	glUniform3fv(location, 1, glm::value_ptr(vector));
}

void Shader::setVec4(const std::string &name, glm::vec4 vector) const
{
	unsigned int location = glGetUniformLocation(id, name.c_str());
	glUniform4fv(location, 1, glm::value_ptr(vector));
}

void Shader::setMat4(const std::string &name, glm::mat4 matrix) const
{
	unsigned int mat = glGetUniformLocation(id, name.c_str());
	glUniformMatrix4fv(mat, 1, false, glm::value_ptr(matrix));
}

void Shader::setSampler2D(const std::string &name, unsigned int texturePointer, int id) const
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_2D, texturePointer);
	this->setInt(name, id);
}
void Shader::setSampler3D(const std::string &name, unsigned int texturePointer, int id) const
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_3D, texturePointer);
	this->setInt(name, id);
}

#include "BaseShader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>


BaseShader::BaseShader(const char * shaderPathChar)
{
	shaderPath = std::string(shaderPathChar);
	std::string shaderCode = loadShaderFromFile(shaderPathChar);
	const char * shaderString = shaderCode.c_str();

	shaderTypeID = getShaderType(shaderPathChar);
	shaderID = glCreateShader(shaderTypeID.typeID);
	glShaderSource(shaderID, 1, &shaderString, NULL);
	glCompileShader(shaderID);
	checkCompileErrors(shaderID, shaderTypeID.name.c_str(), getShaderName(shaderPathChar));

}


bool checkCompileErrors(unsigned int shader, std::string type, std::string shaderName)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR: SHADER" << shaderName << "COMPILATION ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}

	//if (success) {
	//	std::cout << type + " SHADER SUCCESSFULLY COMPILED AND/OR LINKED!" << std::endl;
	//}
	return success;
}

std::string BaseShader::loadShaderFromFile(const char* shaderPath) {
	std::string shaderCode;
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;
		// read file's buffer contents into streams
		shaderStream << shaderFile.rdbuf();
		// close file handlers
		shaderFile.close();
		// convert stream into string
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER " << getShaderName(shaderPath) << " FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	return shaderCode;

}

std::string getShaderName(const char* path) {
	std::string pathstr = std::string(path);
	const size_t last_slash_idx = pathstr.find_last_of("/");
	if (std::string::npos != last_slash_idx)
	{
		pathstr.erase(0, last_slash_idx + 1);
	}
	return pathstr;
}
ShaderType getShaderType(const char* path) {
	std::string type = getShaderName(path);
	const size_t last_slash_idx = type.find_last_of(".");
	if (std::string::npos != last_slash_idx)
	{
		type.erase(0, last_slash_idx + 1);
	}
	if (type == "vert")
		return ShaderType(GL_VERTEX_SHADER, "VERTEX");
	if (type == "frag")
		return ShaderType(GL_FRAGMENT_SHADER, "FRAGMENT");
	if (type == "tes")
		return ShaderType(GL_TESS_EVALUATION_SHADER, "TESS_EVALUATION");
	if (type == "tcs")
		return ShaderType(GL_TESS_CONTROL_SHADER, "TESS_CONTROL");
	if (type == "geom")
		return ShaderType(GL_GEOMETRY_SHADER, "GEOMETRY");
	if (type == "comp")
		return ShaderType(GL_COMPUTE_SHADER, "COMPUTE") ;
}


BaseShader::~BaseShader()
{
	//glDeleteShader(shad);
}

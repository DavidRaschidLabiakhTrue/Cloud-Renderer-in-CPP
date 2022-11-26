#pragma once
#include <string>
#include <glad/glad.h>

struct ShaderType 
{
	ShaderType() : typeID(-1), name("") 
	{
	}
	ShaderType(unsigned int type, std::string name) : typeID(type), name(name)
	{
	}
	unsigned int typeID;
	std::string name;
};

ShaderType getShaderType(const char* path);
std::string getShaderName(const char* shaderPath);
bool checkCompileErrors(unsigned int shader, std::string type, std::string shaderName);


class CoreShaderBasis
{
	public:
		CoreShaderBasis(const char * shaderPathChar);
		virtual ~CoreShaderBasis();

		std::string getName() 
		{
			return getShaderName(shaderPath.c_str());
		}
		unsigned int getShad()
		{
			return shaderID;
		}
		ShaderType getType() 
		{
			return shaderTypeID;
		}

	private:
		std::string loadShaderFromFile(const char* shaderPath);

		std::string shaderPath;


		unsigned int shaderID;
		ShaderType shaderTypeID;
};


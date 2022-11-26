#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <list>

#include "CoreShaderBasis.hpp"


typedef std::list<unsigned int> ShaderList;

class Shader
{
	public:
		Shader(std::string name);
		Shader(std::string name, const char * computeShader);
		Shader * attachShader(CoreShaderBasis s);
		void linkPrograms();

		virtual ~Shader();
		void use();

		// standard GLSL setters
		void uploadFloatToGPU(const std::string& name, float value) const;
		void uploadIntToGPU(const std::string& name, int value) const;
		void uploadBoolToGPU(const std::string &name, bool value) const;
		void uploadVector3ToGPU(const std::string& name, glm::vec3 vector) const;
		void uploadMatrix4ToGPU(const std::string& name, glm::mat4 matrix) const;
		void uploadVector2ToGPU(const std::string &name, glm::vec2 vector) const;
		void uploadVector4ToGPU(const std::string &name, glm::vec4 vector) const;
		void uploadSampler3DToGPU(const std::string& name, unsigned int texturePointer, int id) const;
		void uploadSampler2DToGPU(const std::string &name, unsigned int texturePointer, int id) const;

		unsigned int id;
		bool isProgramLinked;
		bool isComputeShader;
		ShaderList shadersToLink;
		std::string shaderName;
};


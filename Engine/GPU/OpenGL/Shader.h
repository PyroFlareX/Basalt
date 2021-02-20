#pragma once

#include <iostream>
#include "../../Engine/Engine.h"
#include "../../Engine/glad/glad/glad.h"

class Shader
{
public:
	Shader()
	{
		ID = glCreateProgram();
	}

	Shader(const std::string& vertPath, const std::string& fragPath)
	{
		ID = glCreateProgram();
		load(vertPath, fragPath);
	}

	void addShader(const std::string& path, GLenum type)
	{
		std::string source = vn::getFileString(path);
		const char* sourcePtr = source.c_str();

		//Shader Handle Stuff
		unsigned int shader;
		shader = glCreateShader(type);
		glShaderSource(shader, 1, &sourcePtr, NULL);
		glCompileShader(shader);

		//Check Success of Shader
		int  success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR! : Shader compilation failed\n" << infoLog << std::endl;
		}

		//Attach Shader to Program
		glAttachShader(ID, shader);
		shaders.push_back(shader);
	}

	void load(const std::string& vertPath, const std::string& fragPath)
	{
		//Basic Shaders
		addShader(vertPath, GL_VERTEX_SHADER);
		addShader(fragPath, GL_FRAGMENT_SHADER);

		//Link Shader Program
		glLinkProgram(ID);

		//Check Success of Program
		int  success;
		char infoLog[512];
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR! Program Stuff failed!\n" << infoLog << "\n";
		}

		//Delete Shaders
		for (auto& shader : shaders)
		{
			glDeleteShader(shader);
		}
		shaders.clear();
	}

	//Use / Activate Shader
	void use()
	{
		glUseProgram(ID);
	}

	// utility uniform functions
	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setVec3(const std::string &name, vn::vec3 value) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
	}
	void setVec4(const std::string &name, vn::vec4 value) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
	}
	void setMat3(const std::string& name, vn::mat3 value)
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
	void setMat4(const std::string& name, vn::mat4 value)
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}


	//Shader Program:
	unsigned int ID;

private:
	std::vector<unsigned int> shaders;
};
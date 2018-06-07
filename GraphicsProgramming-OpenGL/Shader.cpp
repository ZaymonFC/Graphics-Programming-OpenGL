#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <GLM/mat4x4.hpp>


Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	//
	// ─── LOAD SOURCES FROM DISK ─────────────────────────────────────────────────────
	//
	std::string vertexShaderSource;
	std::string fragmentShaderSource;
	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;

	// Ensure the ifstream objects can throw exceptions
	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vertexShaderFile.open(vertexPath);
		fragmentShaderFile.open(fragmentPath);
		
		std::stringstream vertexShaderStream, fragmentShaderStream;

		// Read file buffer contents into streams
		vertexShaderStream << vertexShaderFile.rdbuf();
		fragmentShaderStream << fragmentShaderFile.rdbuf();

		// Close file handles
		vertexShaderFile.close();
		fragmentShaderFile.close();

		// Convert stream into string
		vertexShaderSource = vertexShaderStream.str();
		fragmentShaderSource = fragmentShaderStream.str();
	}
	catch (std::ifstream::failure &e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}

	const auto vertexShaderCode = vertexShaderSource.c_str();
	const auto fragmentShaderCode = fragmentShaderSource.c_str();

	//
	// ─── COMPILE SHADERS ────────────────────────────────────────────────────────────
	//
	int success;
	char infoLog[512];

	// -- Vertex Shader ---------------------------------------------------------------
	auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
	glCompileShader(vertexShader);

	// Check for and print compile errors
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		fflush(stdout);
	}

	// -- Fragment Shader -------------------------------------------------------------
	auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
	glCompileShader(fragmentShader);

	// Check for and print compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		fflush(stdout);
	}

	//
	// ─── LINK SHADER PROGRAM ────────────────────────────────────────────────────────
	//
	
	ID = glCreateProgram();

	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	
	glLinkProgram(ID);

	// Check for linking error and print if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);

	if(!success)
	{
		glGetProgramInfoLog(ID, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Delete the shaders as they are linked into the shader program
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

auto Shader::Use() -> void
{
	glUseProgram(ID);
}

auto Shader::SetBool(const std::string& name, const bool value) const -> void
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}

auto Shader::SetInt(const std::string& name, const int value) const -> void
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

auto Shader::SetFloat(const std::string& name, const float value) const -> void
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

auto Shader::SetVec2(const std::string &name, const glm::vec2 &value) const -> void
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

auto Shader::SetVec2(const std::string &name, float x, float y) const -> void
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

auto Shader::SetVec3(const std::string &name, const glm::vec3 &value) const -> void
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

auto Shader::SetVec3(const std::string &name, float x, float y, float z) const -> void
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

auto Shader::SetVec4(const std::string &name, const glm::vec4 &value) const -> void
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

auto Shader::SetVec4(const std::string &name, float x, float y, float z, float w) -> void
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

auto Shader::SetMat2(const std::string &name, const glm::mat2 &mat) const -> void
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

auto Shader::SetMat3(const std::string &name, const glm::mat3 &mat) const -> void
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

auto Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const -> void
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

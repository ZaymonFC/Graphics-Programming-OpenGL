#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>


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

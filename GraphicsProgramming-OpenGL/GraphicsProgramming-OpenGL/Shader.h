#pragma once
#include <glad/glad.h>
#include <string>

class Shader
{
public:
	unsigned int ID;
	
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

	// Activate the shaders
	auto Use() -> void;

	// Utility uniform functions
	auto SetBool(const std::string &name, bool value) const -> void;
	auto SetInt(const std::string &name, int value) const -> void;
	auto SetFloat(const std::string &name, float value) const -> void;
};


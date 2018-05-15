#pragma once
#include <glad/glad.h>
#include <string>
#include <GLM/detail/type_vec2.hpp>
#include <GLM/mat4x4.hpp>

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
	auto SetVec2(const std::string & name, const glm::vec2 & value) const -> void;
	auto SetVec2(const std::string & name, float x, float y) const -> void;
	auto SetVec3(const std::string & name, const glm::vec3 & value) const -> void;
	auto SetVec3(const std::string & name, float x, float y, float z) const -> void;
	auto SetVec4(const std::string & name, const glm::vec4 & value) const -> void;
	auto SetVec4(const std::string & name, float x, float y, float z, float w) -> void;
	auto SetMat2(const std::string & name, const glm::mat2 & mat) const -> void;
	auto SetMat3(const std::string & name, const glm::mat3 & mat) const -> void;
	auto SetMat4(const std::string & name, const glm::mat4 & mat) const -> void;
};

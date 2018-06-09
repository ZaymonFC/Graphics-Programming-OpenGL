#pragma once
#include <GLM/mat4x4.hpp>
#include "Mesh.h"

class GameObject
{
public:
	glm::vec3 worldPosition;
	glm::vec3 worldRotation;
	Mesh mesh;

	GameObject(Mesh mesh, glm::vec3 initialPosition, glm::vec3 initialRotation);

	auto Draw(Shader shaderProgram) -> void;

	auto Teleport(glm::vec3 position) -> void;
	auto Translate(float xAmount, float yAmount, float zAmount) -> void;

	auto Rotate(glm::vec3 rotations) -> void;
};


#pragma once
#include <GLM/mat4x4.hpp>
#include "Mesh.h"
#include "Model.h"
#include "FrustumG.h"

class GameObject
{
public:
	glm::vec3 worldPosition;
	glm::vec3 worldRotation;
	glm::vec3 worldScale;
	Model model;

	GameObject(Model model, glm::vec3 initialPosition, glm::vec3 initialRotation, glm::vec3 initialScale);

	auto Draw(Shader shaderProgram, FrustumG & frustum) -> void;
	auto Draw(Shader shaderProgram) -> void;

	auto Teleport(glm::vec3 position) -> void;
	auto Rotate(glm::vec3 rotations) -> void;
	auto Scale(glm::vec3 scale) -> void;
};


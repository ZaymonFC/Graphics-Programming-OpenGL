#include "GameObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include "FrustumG.h"
#include <iostream>

GameObject::GameObject(Model model, glm::vec3 initialPosition, glm::vec3 initialRotation, glm::vec3 initialScale)
{
	GameObject::model = model;
	worldPosition = initialPosition;
	worldRotation = initialRotation;
	worldScale = initialScale;
}

auto GameObject::Draw(Shader shaderProgram, FrustumG & frustum) -> void
{
	auto draw = false;
	// Check all points are inside the frustum
	for (auto &mesh : model.meshes)
	{
		for (auto & vertex : mesh.vertices_)
		{
			if (frustum.pointInFrustum(vertex.Position) == FrustumG::INSIDE)
			{
				draw = true;
				break;
			}
			if (draw) break;
		}
	}

//	if (!draw) return;
	
	if (!draw)
		std::cout << "cull" << std::endl;
	else
		std::cout << "No Cull" << std::endl;

	auto modelMat = glm::mat4();
	modelMat = glm::translate(modelMat, worldPosition);
	modelMat = glm::scale(modelMat, worldScale);
	shaderProgram.SetMat4("model", modelMat);
	model.Draw(shaderProgram);
}


auto GameObject::Draw(Shader shaderProgram) -> void
{
	auto modelMat = glm::mat4();
	modelMat = glm::translate(modelMat, worldPosition);
	modelMat = glm::scale(modelMat, worldScale);
	shaderProgram.SetMat4("model", modelMat);
	model.Draw(shaderProgram);
}

auto GameObject::Teleport(glm::vec3 position) -> void
{

}

auto GameObject::Rotate(glm::vec3 rotations) -> void
{

}

auto GameObject::Scale(glm::vec3 scale) -> void
{

}

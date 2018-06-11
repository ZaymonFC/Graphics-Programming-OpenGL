#pragma once
#include "Vertex.h"
#include <vector>
#include "Texture.h"
#include "Shader.h"

class Mesh
{
public:
	// Mesh Data
	std::vector<Vertex> vertices_;
	std::vector<unsigned int> indices_;
	std::vector<Texture> textures_;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	auto Draw(Shader shader) -> void;

private:
	unsigned int VAO, VBO, EBO;

	auto SetupMesh() -> void;
};


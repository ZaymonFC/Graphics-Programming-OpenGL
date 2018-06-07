#pragma once
#include <glm/detail/type_vec3.hpp>
#include <glm/detail/type_vec2.hpp>


struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

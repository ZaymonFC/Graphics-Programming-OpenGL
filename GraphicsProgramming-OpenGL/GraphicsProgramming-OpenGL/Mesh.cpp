#include "Mesh.h"
#include <glad/glad.h>
#include "Shader.h"
#include <iostream>


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned> indices, std::vector<Texture> textures): 
	vertices_(std::move(vertices)), indices_(std::move(indices)), textures_(std::move(textures))
{
	SetupMesh();
}

auto Mesh::SetupMesh() -> void
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Buffer vertex data to the GPU -----------------------------------------------------------------
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);
	
	// Buffer edge list data to the GPU --------------------------------------------------------------
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

	// Setup the VAO attributes ----------------------------------------------------------------------
	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void *>(nullptr));

	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, Normal)));

	// Vertex Texture Coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, TexCoords)));

	// Vertex Tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, Tangent)));

	// Vertex Tangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, Bitangent)));

	glBindVertexArray(0);
}

auto Mesh::Draw(Shader shaderProgram) -> void
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;


	for (unsigned int i = 0; i < textures_.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		std::string textureNumber;
		const auto textureType = textures_.at(i).type;

		// Increment the counters based on the texture type
		if (textureType == "texture_diffuse")
		{
			textureNumber = std::to_string(diffuseNr++);
		}
		else if (textureType == "texture_specular")
		{
			textureNumber = std::to_string(specularNr++);
		}
		else if (textureType == "texture_normal")
		{
			textureNumber = std::to_string(normalNr++);
		}
		else if (textureType == "texture_height")
		{
			textureNumber = std::to_string(heightNr++);
		}

		shaderProgram.SetFloat(("material." + textureType + textureNumber).c_str(), i);

		glBindTexture(GL_TEXTURE_2D, textures_[i].id);
	}

	// Set the active texture to the first one
	glActiveTexture(GL_TEXTURE0);

	// Draw Mesh -------------------------------------------------------------------------------------
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

}
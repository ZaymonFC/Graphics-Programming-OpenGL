#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "stb_image.h"

auto Model::LoadModel(std::string path) -> void
{
	Assimp::Importer importer;
	const auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		const auto error = std::string("ERROR::ASSIMP::") + importer.GetErrorString();
		std::cout << error << std::endl;
		return;
	}

	directory_ = path.substr(0, path.find_last_of('\\'));

	ProcessNode(scene->mRootNode, scene);
}

// Recursive function to process all of the nodes of an assimp scene object
auto Model::ProcessNode(aiNode* node, const aiScene* scene) -> void
{
	// Process all node's meshes
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		const auto mesh = scene->mMeshes[node->mMeshes[i]];
		meshes_.push_back(ProcessMesh(mesh, scene));
	}

	// Recurse on all children of node
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

auto Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) -> Mesh
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// Read in vertex position, normal and texture coordinates ---------------------------------------
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;

		// Position
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		// Normal
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		// Texture Coordinates
		if (mesh->mTextureCoords[0]) // Check to make sure the mesh has texture coordinates
		{
			glm::vec2 vec2;
			vec2.x = mesh->mTextureCoords[0][i].x;
			vec2.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec2;
		}
		else
		{
			// Blank Tex Coord
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

//		// Tangent
//		vector.x = mesh->mTangents[i].x;
//		vector.y = mesh->mTangents[i].y;
//		vector.z = mesh->mTangents[i].z;
//		vertex.Tangent = vector;
//
//		// Bitangent
//		vector.x = mesh->mBitangents[i].x;
//		vector.y = mesh->mBitangents[i].y;
//		vector.z = mesh->mBitangents[i].z;
//		vertex.Bitangent = vector;

		vertices.push_back(vertex);
	}

	// Read in indices -------------------------------------------------------------------------------
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		auto face = mesh->mFaces[i];
		for (unsigned int i = 0; i < face.mNumIndices; ++i)
		{
			indices.push_back(face.mIndices[i]);
		}
	}

	// Load in diffuse maps and specular maps
	if (mesh->mMaterialIndex >= 0)
	{
		auto material = scene->mMaterials[mesh->mMaterialIndex];

		auto diffuseMaps = LoadTextureMaterials(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		auto specularMaps = LoadTextureMaterials(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		auto normalMaps = LoadTextureMaterials(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		auto heightMaps = LoadTextureMaterials(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

auto Model::LoadTextureMaterials(aiMaterial* material, aiTextureType textureType, std::string typeName) -> std::vector<Texture>
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < material->GetTextureCount(textureType); ++i)
	{
		auto skip = false;
		aiString str;
		material->GetTexture(textureType, i, &str);

		for (auto& loadedTexture : loadedTextures_)
		{
			if (std::strcmp(loadedTexture.path.data(), str.C_Str()) == 0)
			{
				textures.push_back(loadedTexture);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory_);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			loadedTextures_.push_back(texture);
		}
	}
	return textures;
}

Model::Model(const char* path)
{
	LoadModel(path);
}

auto Model::Draw(Shader shaderProgram) -> void
{
	for (auto&& mesh : meshes_)
	{
		mesh.Draw(shaderProgram);
	}
}


auto Model::TextureFromFile(const char *path, const std::string &directory) const -> unsigned int
{
	auto filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	auto *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << directory_ << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
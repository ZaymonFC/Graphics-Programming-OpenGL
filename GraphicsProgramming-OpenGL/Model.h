#pragma once
#include "Shader.h"
#include "Mesh.h"
#include <assimp/scene.h>

class Model
{
private:
	std::string directory_;
	std::vector<Texture> loadedTextures_;

	auto LoadModel(std::string path) -> void;
	auto ProcessNode(aiNode *node, const aiScene * scene) -> void;
	auto ProcessMesh(aiMesh * mesh, const aiScene *scene)->Mesh;
	auto LoadTextureMaterials(aiMaterial * material, aiTextureType textureType, std::string typeName) -> std::vector<Texture>;

public:
	std::vector<Mesh> meshes;
	Model(const char * path);
	Model() = default;

	auto Draw(Shader shaderProgram) -> void;
	auto TextureFromFile(const char* path, const std::string& directory) const -> unsigned int;
};


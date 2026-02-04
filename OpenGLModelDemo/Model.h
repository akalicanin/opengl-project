#ifndef _MOJ_MODEL_H_
#define _MOJ_MODEL_H_

#include "Mesh.h"
#include "Node.h"
#include "Shader.h"
#include "TextureCache.h"

#include "iostream"
#include "unordered_map"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "stb_image.h"

class Model {
public:

	Node* rootNode;

	std::vector<Mesh> meshes;

	Model(const std::string& path) {
		bool success = loadModel(path);
		if (!success) {
			std::cout << "Failed loading model." << std::endl;
		}
	}

	void draw(Shader& shader);

private:

	// directory in which model is located
	std::string directory;

	bool loadModel(const std::string& path);

	void loadAllTexturesFromMaterialIntoCache(const aiScene* scene, aiMaterial* mat);

	Node* processNode(aiNode* node, Node* callingNode);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> processTextures(const aiScene* scene, aiMaterial* mat, aiTextureType type, std::string name);

	void processTexturesForCache(const aiScene* scene, aiMaterial* mat, aiTextureType type, std::string name);

	unsigned int loadTextureFromFile(const char* path, const std::string& directory);

	unsigned int loadEmbeddedTexture(const aiTexture* texture);

	glm::mat4 transformToGLMatrix(aiMatrix4x4 assimpMatrix);

};

#endif
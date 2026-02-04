#include "Model.h"

void Model::draw(Shader& shader) {
	this->rootNode->draw(shader, this->meshes);
}

bool Model::loadModel(const std::string& path) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, 
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenNormals |
		aiProcess_ValidateDataStructure |
		aiProcess_Triangulate |
		aiProcess_FlipUVs
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Error occured while loading model at path: " << path << std::endl;
		std::cout << "Error message is: " << importer.GetErrorString() << std::endl;
		return false;
	}

	directory = path.substr(0, path.find_last_of('/'));

	for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
		aiMaterial* mat = scene->mMaterials[i];
		loadAllTexturesFromMaterialIntoCache(scene, mat);
	}

	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		this->meshes.push_back(processMesh(scene->mMeshes[i], scene));
	}

	for (unsigned int i = 0; i < scene->mRootNode->mNumChildren; i++) {
		this->rootNode = processNode(scene->mRootNode, nullptr);
	}

	std::cout << "Struktura ovog modela: " << path << std::endl;
	std::cout << this->rootNode->name << std::endl;
	for (unsigned int i = 0; i < this->rootNode->numOfChildren; i++) {
		std::cout << "    " << this->rootNode->children[i]->name << std::endl;
		for (unsigned int j = 0; j < this->rootNode->children[i]->numOfChildren; j++) {
			std::cout << "        " << this->rootNode->children[i]->children[j]->name << std::endl;
			for (unsigned int k = 0; k < this->rootNode->children[i]->children[j]->numOfChildren; k++) {
				std::cout << "            " << this->rootNode->children[i]->children[j]->children[k]->name << std::endl;
			}
		}
	}


	return true;
}

void Model::loadAllTexturesFromMaterialIntoCache(const aiScene* scene, aiMaterial* mat) {

	processTexturesForCache(scene, mat, aiTextureType_DIFFUSE, "texture_diffuse");

	processTexturesForCache(scene, mat, aiTextureType_SPECULAR, "texture_specular");

}

Node* Model::processNode(aiNode* node, Node* callingNode) {

	if (!node) return nullptr;

	// da ne saljem svaki put parentTransform.
	// Ako je rootnode True, onda cemo parentTransform staviti da bude glm::mat4(1.0f).
	// U suprotnom, slobodno koristi parent->localTransform.
	bool rootNode = false;
	if (callingNode == nullptr) rootNode = true;

	glm::mat4 parentTransform;
	if (rootNode) {
		parentTransform = glm::mat4(1.0f);
	}
	else {
		parentTransform = callingNode->transformMatrix;
	}

	Node* myNode = new Node;

	std::string nodeName = std::string(node->mName.C_Str());
	myNode->name = nodeName;

	glm::mat4 localTransform = transformToGLMatrix(node->mTransformation);
	myNode->transformMatrix = parentTransform * localTransform;

	myNode->parent = callingNode;

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		myNode->meshIndices.push_back(node->mMeshes[i]);
	}

	myNode->numOfChildren = node->mNumChildren;
	myNode->numOfMeshIndices = node->mNumMeshes;

	if (node->mChildren) {
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			Node* childNode = processNode(node->mChildren[i], myNode);
			myNode->children.push_back(childNode);
		}
	}

	return myNode;
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		if (mesh->HasNormals()) {
			vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		}
		else {
			vertex.Normal = glm::vec3(0.0f);
		}
		if (mesh->HasTextureCoords(0)) {
			vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}


	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
			indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}


	if (mesh->mMaterialIndex >= 0) {

		aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = processTextures(scene, mat, aiTextureType_DIFFUSE, "texture_diffuse");

		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = processTextures(scene, mat, aiTextureType_SPECULAR, "texture_specular");

		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::processTextures(const aiScene* scene, aiMaterial* mat, aiTextureType type, std::string name) {

	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {

		bool skip = false;
		aiString str;
		mat->GetTexture(type, i, &str);

		if (*str.C_Str() == '*') {
			auto it = TextureCache::getCache().find(std::string(scene->GetEmbeddedTexture(str.C_Str())->mFilename.C_Str()));
			if (it != TextureCache::getCache().end()) {
				skip = true;
				textures.push_back(it->second);
			}

			if (!skip) {
				// Ovo je embedded format koji svoje materijale drzi unutar sebe, ne odvojeno.
				const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());
				Texture tex;
				tex.type = name;
				tex.path = std::string(embeddedTexture->mFilename.C_Str());
				tex.id = loadEmbeddedTexture(embeddedTexture);
				textures.push_back(tex);
				TextureCache::getCache().emplace(tex.path, tex);
			}

		}
		else {
			auto it = TextureCache::getCache().find(std::string(str.C_Str()));
			if (it != TextureCache::getCache().end()) {
				skip = true;
				textures.push_back(it->second);
			}

			if (!skip) {
				Texture tex;
				tex.type = name;
				tex.path = std::string(str.C_Str());
				tex.id = loadTextureFromFile(str.C_Str(), this->directory);
				textures.push_back(tex);
				TextureCache::getCache().emplace(tex.path, tex);
			}
		}
	}

	return textures;
}

void Model::processTexturesForCache(const aiScene* scene, aiMaterial* mat, aiTextureType type, std::string name) {
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {

		bool skip = false;
		aiString str;
		mat->GetTexture(type, i, &str);

		if (*str.C_Str() == '*') {
			auto it = TextureCache::getCache().find(std::string(scene->GetEmbeddedTexture(str.C_Str())->mFilename.C_Str()));
			std::cout << "CACHE::EMBEDD::pokusaj pristupa teksturi: " << scene->GetEmbeddedTexture(str.C_Str())->mFilename.C_Str() << "..." << std::endl;
			if (it != TextureCache::getCache().end()) {
				skip = true;
				std::cout << "CACHE::EMBEDD::tekstura nadjena!" << std::endl;
			}

			if (!skip) {
				// Ovo je embedded format koji svoje materijale drzi unutar sebe, ne odvojeno.
				std::cout << "CACHE::EMBEDD::tekstura nije nadjena.. " << std::endl;
				const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());
				Texture tex;
				tex.type = name;
				tex.path = std::string(embeddedTexture->mFilename.C_Str());
				tex.id = loadEmbeddedTexture(embeddedTexture);
				TextureCache::getCache().emplace(tex.path, tex);
				std::cout << "CACHE::EMBEDD:uspesno kesiranje teksture: " << tex.type + " " + tex.path << std::endl;
			}

		}
		else {
			auto it = TextureCache::getCache().find(std::string(str.C_Str()));
			std::cout << "CACHE::Looking for texture: " << str.C_Str() << "..." << std::endl;
			if (it != TextureCache::getCache().end()) {
				skip = true;
				std::cout << "CACHE::Texture found" << std::endl;
			}

			if (!skip) {
				Texture tex;
				tex.type = name;
				tex.path = std::string(str.C_Str());
				tex.id = loadTextureFromFile(str.C_Str(), this->directory);
				TextureCache::getCache().emplace(tex.path, tex);
				std::cout << "CACHE::Texture cached: " << tex.type + " " + tex.path << std::endl;
			}
		}
	}
}

unsigned int Model::loadTextureFromFile(const char* path, const std::string& directory) {
	
	std::string pathString = std::string(path);
	std::string texPath = directory + "/" + pathString;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int texWidth, texHeight, texNumberOfChannels;
	unsigned char* texData = stbi_load(texPath.c_str(), &texWidth, &texHeight, &texNumberOfChannels, 0);

	if (texData) {

		GLenum imageformat = GL_RGB;
		if (texNumberOfChannels == 1) {
			imageformat = GL_RED;
		}
		if (texNumberOfChannels == 3) {
			imageformat = GL_RGB;
		}
		if (texNumberOfChannels == 4) {
			imageformat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, imageformat, texWidth, texHeight, 0, imageformat, GL_UNSIGNED_BYTE, texData);
		//std::cout << "Generisana tekstura! Velicina je: " << texWidth << " * " << texHeight << " = " << texWidth * texHeight << std::endl;
		//std::cout << "Ucitana tekstura je: " << texPath << std::endl;
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cerr << "Failed to load texture at path: " << texPath << std::endl;
	}

	stbi_image_free(texData);
	glBindTexture(GL_TEXTURE_2D, 0); // unbinding je opcionalan uvek

	return textureID;
}

unsigned int Model::loadEmbeddedTexture(const aiTexture* texture) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int texWidth, texHeight, texNumberOfChannels;
	unsigned char* texData;

	if (texture->mHeight == 0) {
		texData = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &texWidth, &texHeight, &texNumberOfChannels, 0);
	}
	else {
		texData = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth * texture->mHeight, &texWidth, &texHeight, &texNumberOfChannels, 0);
	}

	if (texData) {

		GLenum imageformat = GL_RGB;
		if (texNumberOfChannels == 1) {
			imageformat = GL_RED;
		}
		if (texNumberOfChannels == 3) {
			imageformat = GL_RGB;
		}
		if (texNumberOfChannels == 4) {
			imageformat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, imageformat, texWidth, texHeight, 0, imageformat, GL_UNSIGNED_BYTE, texData);
		//std::cout << "Generisana tekstura! Velicina je: " << texWidth << " * " << texHeight << " = " << texWidth * texHeight << std::endl;
		//std::cout << "Ucitana tekstura je: " << texture->mFilename.C_Str() << std::endl;
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cerr << "Failed to load Embedded texture of name: " << texture->mFilename.C_Str() << std::endl;
	}

	stbi_image_free(texData);
	glBindTexture(GL_TEXTURE_2D, 0); // unbinding je opcionalan uvek

	return textureID;
}

glm::mat4 Model::transformToGLMatrix(aiMatrix4x4 assimpMatrix) {
	return glm::mat4(
		assimpMatrix.a1, assimpMatrix.b1, assimpMatrix.c1, assimpMatrix.d1,
		assimpMatrix.a2, assimpMatrix.b2, assimpMatrix.c2, assimpMatrix.d2,
		assimpMatrix.a3, assimpMatrix.b3, assimpMatrix.c3, assimpMatrix.d3,
		assimpMatrix.a4, assimpMatrix.b4, assimpMatrix.c4, assimpMatrix.d4
	);
}

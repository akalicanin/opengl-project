#ifndef _MOJ_MESH_H_
#define _MOJ_MESH_H_

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "iostream"
#include "vector"
#include "string"

#include "Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	// Name, like texture_diffuse or texture_specular. Will be followed by a number (texture_diffuse1)
	std::string type;
	// Ako je tekstura embedded tipa, ovo je Filename.
	std::string path;
};

class Mesh {
	typedef unsigned int uint;
public:
	uint VAO;

	std::vector<Vertex> vertices;
	std::vector<uint> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> Vertices, std::vector<uint> Indices, std::vector<Texture> Textures) : vertices(Vertices), indices(Indices), textures(Textures) {
		setupMesh();
	}

	void draw(Shader& shader, glm::mat4 transform);

private:

	uint VBO, EBO;

	void setupMesh();

};

#endif

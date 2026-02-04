#include "Mesh.h"

void Mesh::draw(Shader& shader, glm::mat4 transform) {
	int numberOfDiffuse = 1;
	int numberOfSpecular = 1;

	for (int i = 0; i < this->textures.size(); i++) {

		std::string samplerIndex = "material.";

		if (textures[i].type == "texture_diffuse") {
			samplerIndex += textures[i].type + std::to_string(numberOfDiffuse++);
		}
		else if (textures[i].type == "texture_specular") {
			samplerIndex += textures[i].type + std::to_string(numberOfSpecular++);
		}

		shader.setInt(samplerIndex.c_str(), i);

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
		
	}
	glActiveTexture(GL_TEXTURE0);

	shader.use();
	shader.setMat4("model", transform);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * this->indices.size(), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	
}

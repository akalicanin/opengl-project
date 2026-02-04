#include "Node.h"

void Node::draw(Shader& shader, std::vector<Mesh>& modelMeshes) {
	for (unsigned int i = 0; i < this->numOfMeshIndices; i++) {
		modelMeshes[this->meshIndices[i]].draw(shader, this->transformMatrix);
	}

	for (unsigned int i = 0; i < this->numOfChildren; i++) {
		this->children[i]->draw(shader, modelMeshes);
	}
}

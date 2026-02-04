#ifndef _MOJ_NODE_H_
#define _MOJ_NODE_H_

#include <iostream>
#include <string>
#include <vector>

#include "Shader.h"
#include "Mesh.h"

#include "glad/glad.h"
#include "glm/glm.hpp"

class Node {
	typedef unsigned int uint;
public:

	std::string name;

	glm::mat4 transformMatrix;

	std::vector<Node*> children;

	Node* parent;

	std::vector<uint> meshIndices;

	uint numOfMeshIndices;

	uint numOfChildren;

	void draw(Shader& shader, std::vector<Mesh>& modelMeshes);

	void deleteNode() {
		for (unsigned int i = 0; i < this->numOfChildren; i++) {
			this->children[i]->deleteNode();
		}

		delete(this);
	}

private:

	~Node() {
		this->deleteNode();
	}

};

#endif


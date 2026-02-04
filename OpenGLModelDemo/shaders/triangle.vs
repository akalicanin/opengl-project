#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aVertexColors;
layout(location = 2) in vec2 aTexCoords;

out vec2 vsTexCoords;

uniform mat4 u_Model;

void main() {

	gl_Position = u_Model * vec4(aPosition, 1.0f);
	vsTexCoords = aTexCoords;

}
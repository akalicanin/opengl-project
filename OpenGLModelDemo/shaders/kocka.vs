#version 330 core
layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec2 vs_TexCoords;

void main() {
	gl_Position = u_Projection * u_View * u_Model * vec4(a_Pos, 1.0f);
	vs_TexCoords = a_TexCoords;
}
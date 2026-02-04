#ifndef _MOJ_SHADER_H_
#define _MOJ_SHADER_H_

#include "glad/glad.h"
#include "glm/glm.hpp"
#include <string>



class Shader {

typedef unsigned int uint;

public:
	uint programID;

	// Constructor za shader.
	// Vertex shader path, fragment shader path
	Shader(std::string vshaderpath, std::string fshaderpath);

	void use() const;

	void setBool(const GLchar* uniformName, bool value) const;

	void setInt(const GLchar* uniformName, int value) const;

	void setFloat(const GLchar* uniformName, float value) const;

	void set3Float(const GLchar* uniformName, float value1, float value2, float value3) const;

	void setVec2(const GLchar* uniformName, const glm::vec2 &glmVec2value) const;

	void setVec2(const GLchar* uniformName, float x, float y) const;

	void setVec3(const GLchar* uniformName, const glm::vec3 &glmVec3value) const;

	void setVec3(const GLchar* uniformName, float x, float y, float z) const;

	void setVec4(const GLchar* uniformName, const glm::vec4 &glmVec4value) const;

	void setVec4(const GLchar* uniformName, float x, float y, float z, float w) const;

	void setMat2(const GLchar* uniformName, const glm::mat2 &mat) const;

	void setMat3(const GLchar* uniformName, const glm::mat3 &mat) const;

	void setMat4(const GLchar* uniformName, const glm::mat4 &mat) const;

private:

	std::string readShaderRaw(std::string shaderpath) const;

	static std::string readShaderSource(std::string shaderpath);

	void checkCompileError(uint ID, std::string errorType);
};
#endif


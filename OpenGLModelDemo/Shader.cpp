#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(std::string vshaderpath, std::string fshaderpath) {
	std::string rawVshader = readShaderRaw(vshaderpath);
	std::string rawFshader = readShaderRaw(fshaderpath);

	const char* pRawVShader = rawVshader.c_str();
	const char* pRawFShader = rawFshader.c_str();

	uint vshader, fshader, shaderprogram;

	vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, &pRawVShader, NULL);
	glCompileShader(vshader);
	checkCompileError(vshader, "Vertex Shader");

	fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, &pRawFShader, NULL);
	glCompileShader(fshader);
	checkCompileError(fshader, "Fragment Shader");

	shaderprogram = glCreateProgram();
	glAttachShader(shaderprogram, vshader);
	glAttachShader(shaderprogram, fshader);
	glLinkProgram(shaderprogram);
	checkCompileError(shaderprogram, "PROGRAM");

	this->programID = shaderprogram;

	glDeleteShader(vshader);
	glDeleteShader(fshader);
}

std::string Shader::readShaderRaw(std::string shaderpath) const {
	std::ifstream rawShaderFile;
	std::stringstream rawShaderText;
	std::string shaderCode = "";
	rawShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		rawShaderFile.open(shaderpath);
		rawShaderText << rawShaderFile.rdbuf();
		rawShaderFile.close();
		shaderCode = rawShaderText.str();
	}
	catch (std::ifstream::failure& e) {
		std::cout << "ERROR While reading shader raw at: " << shaderpath << " error is " << e.what() << std::endl;
	}

	return shaderCode;
}

// Don't use, outdated, use readShaderRaw instead
std::string Shader::readShaderSource(std::string shaderpath) {
	std::ifstream rawshaderfile;
	rawshaderfile.open(shaderpath);
	if (!rawshaderfile.is_open()) {
		std::cerr << "Raw shader file at location: " << shaderpath << " failed to open!" << std::endl;
		exit(250);
	}

	std::stringstream rawshadertext;
	rawshadertext << rawshaderfile.rdbuf();

	return rawshadertext.str();
}

void Shader::checkCompileError(uint ID, std::string errorType) {
	int success;
	char log[512];
	if (errorType == "PROGRAM") {
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (success == GL_FALSE) {
			glGetProgramInfoLog(ID, 512, NULL, log);
			glDeleteProgram(ID);
			std::cout << "Shader program linking failed, of type " << errorType << " error is: " << log << std::endl;
		}
	}
	else {
		glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE) {
			glGetShaderInfoLog(ID, 512, NULL, log);
			glDeleteShader(ID);
			std::cout << "Shader compilation failed, of type " << errorType << " error is: " << log << std::endl;
		}
	}
}

void Shader::use() const {
	glUseProgram(this->programID);
}

void Shader::setBool(const GLchar* uniformName, bool value) const {
	glUniform1i(glGetUniformLocation(this->programID, uniformName), (int)value);
}

void Shader::setInt(const GLchar* uniformName, int value) const {
	glUniform1i(glGetUniformLocation(this->programID, uniformName), value);
}

void Shader::setFloat(const GLchar* uniformName, float value) const {
	glUniform1f(glGetUniformLocation(this->programID, uniformName), value);
}

void Shader::set3Float(const GLchar* uniformName, float value1, float value2, float value3) const {
	glUniform3f(glGetUniformLocation(this->programID, uniformName), value1, value2, value3);
}

void Shader::setVec2(const GLchar* uniformName, const glm::vec2& glmVec2value) const {
	glUniform2fv(glGetUniformLocation(this->programID, uniformName), 1, &glmVec2value[0]);
}

void Shader::setVec2(const GLchar* uniformName, float x, float y) const {
	glUniform2f(glGetUniformLocation(this->programID, uniformName), x, y);
}

void Shader::setVec3(const GLchar* uniformName, const glm::vec3& glmVec3value) const {
	glUniform3fv(glGetUniformLocation(this->programID, uniformName), 1, &glmVec3value[0]);
}

void Shader::setVec3(const GLchar* uniformName, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(this->programID, uniformName), x, y, z);
}

void Shader::setVec4(const GLchar* uniformName, const glm::vec4& glmVec4value) const {
	glUniform4fv(glGetUniformLocation(this->programID, uniformName), 1, &glmVec4value[0]);
}

void Shader::setVec4(const GLchar* uniformName, float x, float y, float z, float w) const {
	glUniform4f(glGetUniformLocation(this->programID, uniformName), x, y, z, w);
}

void Shader::setMat2(const GLchar* uniformName, const glm::mat2& mat) const {
	glUniformMatrix2fv(glGetUniformLocation(this->programID, uniformName), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const GLchar* uniformName, const glm::mat3& mat) const {
	glUniformMatrix3fv(glGetUniformLocation(this->programID, uniformName), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const GLchar* uniformName, const glm::mat4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(this->programID, uniformName), 1, GL_FALSE, &mat[0][0]);
}

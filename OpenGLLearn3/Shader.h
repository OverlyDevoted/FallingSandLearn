#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID;
	Shader(const char* computePath);
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();
	unsigned int compileShaderFile(const char* filePath, GLuint type);
	std::string readFromFile(std::string fileName);
	
	void use();

	void setUniform4f(std::string locationName, float values[]);
	void setUniform4m(std::string locationName, glm::mat4 value);
	void setUniform1f(std::string locationName, float value);
	void setUniform1uint(std::string locationName, unsigned int value);
	void setUniform3f(std::string locationName, float* values);
	void setUniform3f(std::string locationName, glm::vec3 values);
private: 
	
};


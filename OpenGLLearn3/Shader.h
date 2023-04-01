#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID;
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();
	unsigned int compileShaderFile(const char* filePath, GLuint type);
	std::string readFromFile(std::string fileName);
	
	void use();
	void setUniform4v(std::string locationName, float values[]);
private: 
	
};


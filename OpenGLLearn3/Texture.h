#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <iostream>

#include "src/stb_image.h"
class Texture
{
public:
	Texture(std::string texturePath, unsigned int format, bool flip);
	unsigned int textureID;
	int width;
	int height;
	int nrChannels;
	void use(GLuint textureSlot);
};


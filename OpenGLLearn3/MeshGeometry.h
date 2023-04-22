#pragma once
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class MeshGeometry
{
public:
	MeshGeometry();
	~MeshGeometry();
	void drawRectangle();
	void bindRectangle();

	void bindCube();
	void drawCubeManual();

private:
	unsigned int cubeVao;
	unsigned int cubeVbo;
	unsigned int cubeEbo;
	unsigned int rectangleVao;
	unsigned int rectangleVbo;
	unsigned int rectangleEbo;
	
	void createRectangle();
	void createCube();
};


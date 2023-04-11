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
	void drawCube();
	void bindCube();
	void drawCubeManual();

private:
	unsigned int cubeVao;
	unsigned int cubeVbo;
	unsigned int cubeEbo;

	void createRectangle();
	void createCube();
};


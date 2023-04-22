#include "MeshGeometry.h"
#include <iostream>
MeshGeometry::MeshGeometry()
{
	createCube();
}

MeshGeometry::~MeshGeometry()
{
	glDeleteBuffers(1, &cubeVao);
	glDeleteBuffers(1, &cubeVbo);
	glDeleteBuffers(1, &cubeEbo);
	glDeleteBuffers(1, &rectangleVao);
	glDeleteBuffers(1, &rectangleVbo);
	glDeleteBuffers(1, &rectangleEbo);
	
}

void MeshGeometry::drawRectangle()
{
	glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_INT, 0);
}

void MeshGeometry::bindRectangle()
{
	glBindVertexArray(rectangleVao);
}


void MeshGeometry::createRectangle()
{
	unsigned int rectangle_elements[] = {
		// front
		0, 1, 2,
		2, 3, 0,
	};
	float rectangle_vertices[] = {
		// front
		-1.0, -1.0,  1.0,
		 1.0, -1.0,  1.0,
		 1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
	};
	glGenVertexArrays(1, &cubeVao);
	glGenBuffers(1, &cubeVbo);
	glGenBuffers(1, &cubeEbo);

	glBindVertexArray(cubeVao);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), rectangle_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangle_elements), rectangle_elements, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshGeometry::createCube()
{
	/*unsigned int cube_elements[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};
	float cube_vertices[] = {
		// front
		-1.0, -1.0,  1.0,
		 1.0, -1.0,  1.0,
		 1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		// back
		-1.0, -1.0, -1.0,
		 1.0, -1.0, -1.0,
		 1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0
	};*/
	float cube_vertices[] = {
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,


		-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f,


		1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,


		-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,


		-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,


		-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f
	};

	unsigned int cube_elements[] = {
		8,
		9,
		10,
		9,
		11,
		10,

		14,
		13,
		12,
		14,
		15,
		13,

		1,
		2,
		0,
		3,
		2,
		1,

		4,
		6,
		5,
		5,6,7,

		17,18,16,
		19,18,17,

		20,22,21,
		21,22,23
	};
	
	glGenVertexArrays(1, &cubeVao);
	glGenBuffers(1, &cubeVbo);
	glGenBuffers(1, &cubeEbo);

	glBindVertexArray(cubeVao);
	
	glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void MeshGeometry::drawCubeManual()
{
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void MeshGeometry::bindCube()
{
	glBindVertexArray(cubeVao);
}


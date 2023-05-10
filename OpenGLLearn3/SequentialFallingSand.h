#pragma once
#include <iostream>
#include <functional>
#include <vector>
#include <utility>
#include <algorithm>
//GLM(mathematics)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>

#include "IFallingSand.h"
#include "Shader.h"
#include "MeshGeometry.h"

//WORTH NOTING: THE CELL COORDINATES ARE STORED IN THIS FASHION: i which usually stands for the row index is stored in x and j (cols) is stored in y
class SequentialFallingSand : public IFallingSand
{
public:
	SequentialFallingSand();
	~SequentialFallingSand() override;
	std::pair<int, int> InitializeSpace(const unsigned int& size, const bool& random) override;
	float IterateSpace();
	void IterateThroughSpace();

	void GetNeighboarhood(unsigned int& index);
	void SetPixel(const glm::uvec3& pos, const unsigned int& type);
	unsigned int GetCellCount() const;
	void RenderSpace(const glm::mat4& view, const glm::mat4& projection) override;
	void DeallocateSpace() override;

	
private:
	MeshGeometry* geometry;
	Shader* cubeShader;
	glm::vec3 lightPos;
	unsigned char* space;
	unsigned int cur_cells = 0;
	std::vector <std::pair<size_t, size_t>> space_changes;

	std::pair<int, int> CreateSpace(const int& size, const bool& random);
	void MakeChange(const size_t& from, const size_t& to);
	void CommitChanges();
	size_t ConvertVec3ToIndex(const glm::uvec3& pos);
	glm::uvec3 ConvertIndexToVec3(const size_t& index);
	bool CheckBelow(const size_t& index);
	bool CheckFront(const size_t& index, const size_t& swap);
	bool CheckBack(const size_t& index, const size_t& swap);
	bool CheckLeft(const size_t& index, const size_t& swap);
	bool CheckRight(const size_t& index, const size_t& swap);
	//not really for this class, but needed
	unsigned int ClampInSpace(unsigned int value);
};
//cells with higher value will replace lower value cells 

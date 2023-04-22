#pragma once
#include <iostream>
#include <functional>
#include <vector>
#include <utility>
#include <algorithm>
//GLM(mathematics)
//WORTH NOTING: THE CELL COORDINATES ARE STORED IN THIS FASHION: i which usually stands for the row index is stored in x and j (cols) is stored in y
#include <glm/glm.hpp>
#include <cstdlib>

class FallingSandHelper
{
public:
	FallingSandHelper();
	~FallingSandHelper();
	void InitializeSpace(const unsigned int& size);
	void IterateSpace();
	void IterateThroughSpace(std::function<void(glm::uvec3, unsigned char)> renderFunction);
	unsigned int GetSpaceSize() const;
	//should probably abstract cell
	void GetNeighboarhood(unsigned int& index);
	unsigned int GetStartingCells() const;
	void SetPixel(const glm::uvec3& pos, const unsigned int& type);

private:
	unsigned int size_min = 3;
	unsigned int size;
	unsigned int size_sq;
	unsigned int size_total;
	unsigned int starting_cells = 0;
	unsigned char* space;
	std::vector <std::pair<size_t, size_t>> space_changes;
	
	void AllocateEmptySpace(const int &size);
	void Deallocate2DSpace();
	void MakeChange(const size_t& from, const size_t& to);
	void CommitChanges();
	size_t ConvertVec3ToIndex(const glm::uvec3& pos);
	glm::uvec3 ConvertIndexToVec3(const size_t& index);
};
//cells with higher value will replace lower value cells 
enum CellType {
	_VOID = 0,
	_EMPTY = 1,
	_WATER = 3,
	_SAND = 2,
};
enum CellMove {
	_STAY = 0,
	_BOT_MID = 1,
	_BOT_LEFT = 2,
	_BOT_RIGHT = 3,
	_BOT_FORW = 4,
	_BOT_BACK = 5
};
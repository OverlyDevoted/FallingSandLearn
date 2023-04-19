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
	void InitializeSpace(const unsigned int &new_row, const unsigned int &new_col);
	void IterateSpace();
	void IterateThroughSpace(std::function<void(int, int, unsigned char)> renderFunction);
	glm::vec3 GetSpaceSize() const;
	//should probably abstract cell
	unsigned char GetNeighboarhood(const int& x, const int& y);
	unsigned int GetStartingCells() const;
	void SetPixel(const int& x, const int& y, const unsigned int& type);

private:
	unsigned int rows = 100, cols = 100;
	unsigned int min_row = 3, min_col = 3;
	unsigned int starting_cells = 0;
	unsigned char** space;
	std::vector < std::pair<glm::uvec2, glm::uvec2>> space_changes;
	void Deallocate2DSpace();
	void AllocateEmptySpace(const int &new_row, const int &new_col);
	void MakeChange(const glm::uvec2 &from, const glm::uvec2&to);
	void CommitChanges();
	unsigned int GetCellAt(const glm::uvec2& pos);
	bool IsBounds(const glm::uvec2& pos);
};
enum CellType {
	_VOID = 0,
	_EMPTY = 1,
	_WATER = 2,
	_SAND = 3,
};
enum CellMove {
	_STAY = 0,
	_BOT_MID = 1,
	_BOT_LEFT = 2,
	_BOT_RIGHT = 3,
	_MID_LEFT = 4,
	_MID_RIGHT = 5,
};
#pragma once
#include <iostream>
#include <functional>
#include <vector>
//GLM(mathematics)
#include <glm/glm.hpp>
#include <cstdlib>

class FallingSandHelper
{
public:
	FallingSandHelper();
	~FallingSandHelper();
	void Initialize2DSpace(const unsigned int &new_row, const unsigned int &new_col);
	void IterateSpace(std::function<void(int, int, unsigned char)> renderFunction);
	glm::vec3 GetSpaceSize() const;
	unsigned char GetNeighboarhood(const int& x, const int& y, const char& value);
	unsigned int GetStartingCells() const;

private:
	unsigned int rows = 100, cols = 100;
	unsigned int min_row = 10, min_col = 10;
	unsigned int starting_cells = 0;
	unsigned char** space;
	unsigned char** spaceB;
	void Deallocate2DSpace();
	void AllocateEmpty2DSpace(const int &new_row, const int &new_col);
	void SwapArrays(unsigned char**& to, unsigned char**& from);
};
enum CellType {
	_VOID = 0,
	_EMPTY = 1,
	_SAND = 2,
	_WATER = 3
};
enum CellMove {
	_STAY = 0,
	_BOT_MID = 1,
	_BOT_LEFT = 2,
	_BOT_RIGHT = 3
};
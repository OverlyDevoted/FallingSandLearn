#pragma once
#include <iostream>
#include <functional>
class FallingSandHelper
{
public:
	FallingSandHelper();
	~FallingSandHelper();
	void Initialize2DSpace(int new_row, int new_col);
	void IterateSpace(std::function<void(int, int)> lambda);
private:
	unsigned int rows = 100, cols = 100;
	unsigned int min_row = 50, min_col = 100;
	int** space;
	void Deallocate2DSpace();
	void AllocateEmpty2DSpace(int new_row, int new_col);
};


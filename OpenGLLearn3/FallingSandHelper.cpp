#include "FallingSandHelper.h"

FallingSandHelper::FallingSandHelper()
{
    AllocateEmpty2DSpace(rows, cols);
}

FallingSandHelper::~FallingSandHelper()
{
    Deallocate2DSpace();
}

void FallingSandHelper::Initialize2DSpace(int new_row, int new_col)
{
    if (new_col < min_col || new_row < min_row)
    {
        std::cout << "Couldn't create simulation space. Defined dimensions are too small" << std::endl;
        return;
    }
    Deallocate2DSpace();

}

void FallingSandHelper::IterateSpace(std::function<void(int, int)> lambda)
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            lambda(i, j);
        }
    }
}

void FallingSandHelper::Deallocate2DSpace()
{
    for (int i = 0; i < rows; i++) {
        delete[]space[i];
    }
    delete[] space;
}

void FallingSandHelper::AllocateEmpty2DSpace(int new_row, int new_col)
{
    space = new int* [rows];
    for (int i = 0; i < rows; i++) {
        space[i] = new int[cols];
        for (int j = 0; j < cols; j++) {
            space[i][j] = 0;
        }
    }
}

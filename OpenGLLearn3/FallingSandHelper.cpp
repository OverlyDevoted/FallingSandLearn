#include "FallingSandHelper.h"

FallingSandHelper::FallingSandHelper()
{
    AllocateEmpty2DSpace(rows, cols);
}

FallingSandHelper::~FallingSandHelper()
{
    Deallocate2DSpace();
}

void FallingSandHelper::Initialize2DSpace(const unsigned int& new_row, const unsigned int& new_col)
{
    if (new_col < min_col || new_row < min_row)
    {
        std::cout << "Couldn't create simulation space. Defined dimensions are too small" << std::endl;
        return;
    }
    Deallocate2DSpace();
    AllocateEmpty2DSpace(new_row, new_col);
}

void FallingSandHelper::IterateSpace(std::function<void(int, int, unsigned char)> renderFunction)
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            unsigned char cell = space[i][j];
            //render
            renderFunction(j, i, cell);
            unsigned char tempVal = GetNeighboarhood(j, i, cell);
            //calculate each cell
            
            switch (tempVal)
            {
                case _BOT_MID:
                    spaceB[i][j] = _EMPTY;
                    spaceB[i + 1][j] = cell;
                    break;
                case _BOT_LEFT:
                    spaceB[i][j] = _EMPTY;
                    spaceB[i + 1][j - 1] = cell;
                    break;
                case _BOT_RIGHT:
                    spaceB[i][j] = _EMPTY;
                    spaceB[i + 1][j + 1] = cell;
                    break;
                default:
                    spaceB[i][j] = cell;
                    break;
            }
            
        }
    }
    //we need a good swap function cuz this was aint it brother
}

glm::vec3 FallingSandHelper::GetSpaceSize() const
{
    return glm::vec3((float)rows, (float)cols, 0.0f);
}
unsigned char FallingSandHelper::GetNeighboarhood(const int& x, const int& y, const char& value)
{
    //#1 optimisation:
    //  - We could also try precalculating and storing neighbor positions in order to just fetch data using instead of always running ifs
 
    switch (value)
    {
        case _SAND:
            if (y + 1 < rows)
            {
                if (space[y + 1][x] == _EMPTY)
                    return _BOT_MID;
                if (x - 1 >= 0 && space[y + 1][x - 1] == _EMPTY)
                    return _BOT_LEFT;
                if (x + 1 < cols && space[y + 1][x + 1] == _EMPTY)
                    return _BOT_RIGHT;
            }
            return _STAY;
            break;
        default:
            return _STAY;
            break;
    }
}

unsigned int FallingSandHelper::GetStartingCells() const
{
    return starting_cells;
}

void FallingSandHelper::Deallocate2DSpace()
{
    for (int i = 0; i < rows; i++) {
        delete[]space[i];
        delete[]spaceB[i];
    }
    delete[] space;
    delete[] spaceB;
}

void FallingSandHelper::AllocateEmpty2DSpace(const int& new_row, const int& new_col)
{
    starting_cells = 0;
    // set the random seed
    std::srand(std::time(nullptr));

    rows = new_row;
    cols = new_col;
    space = new unsigned char* [rows];
    spaceB = new unsigned char* [rows];
    std::cout << "Generating " << rows * cols << " number of cells." << std::endl;
    for (int i = 0; i < rows; i++) {
        space[i] = new unsigned char[cols];
        spaceB[i] = new unsigned char[cols];
        for (int j = 0; j < cols; j++) {
            unsigned char cell = std::rand() % 2 + 1;
            if (cell == _SAND)
                starting_cells++;
  
            space[i][j] = cell;
            spaceB[i][j] = space[i][j];
        }
    }
}

void FallingSandHelper::SwapArrays(unsigned char**& to, unsigned char**& from)
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            space[i][j] = spaceB[i][j];
        }
    }
}

#include "FallingSandHelper.h"

FallingSandHelper::FallingSandHelper()
{
    AllocateEmptySpace(rows, cols);
}

FallingSandHelper::~FallingSandHelper()
{
    Deallocate2DSpace();
}

void FallingSandHelper::InitializeSpace(const unsigned int& new_row, const unsigned int& new_col)
{
    if (new_col < min_col || new_row < min_row)
    {
        std::cout << "Couldn't create simulation space. Defined dimensions are too small" << std::endl;
        return;
    }
    Deallocate2DSpace();
    AllocateEmptySpace(new_row, new_col);
}

void FallingSandHelper::IterateSpace(std::function<void(int, int, unsigned char)> renderFunction)
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            unsigned char cell = space[i][j];
            std::cout << (int)cell << " ";
            //render
            renderFunction(j, i, cell);
            unsigned char tempVal = GetNeighboarhood(j, i, cell);
            //gather changes
            switch (tempVal)
            {
                case _BOT_MID:
                    MakeChange(glm::uvec2(j, i), glm::uvec2(j, i + 1));
                    break;
                case _BOT_LEFT:
                    MakeChange(glm::uvec2(j, i), glm::uvec2(j - 1, i + 1));
                    break;
                case _BOT_RIGHT:
                    MakeChange(glm::uvec2(j, i), glm::uvec2(j + 1, i + 1));
                    break;
                default:
                    break;
            }

        }
        std::cout << std::endl;
    }
    std::cout << "------------------------------------------------" << std::endl;
    //commit changes
    CommitChanges();
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
    }
    delete[] space;
}

void FallingSandHelper::AllocateEmptySpace(const int& new_row, const int& new_col)
{
    starting_cells = 0;
    // set the random seed
    std::srand(std::time(nullptr));

    rows = new_row;
    cols = new_col;
    space = new unsigned char* [rows];
    std::cout << "Generating " << rows * cols << " number of cells." << std::endl;
    for (int i = 0; i < rows; i++) {
        space[i] = new unsigned char[cols];
        for (int j = 0; j < cols; j++) {
            unsigned char cell = std::rand() % 2 + 1;
            if (cell == _SAND)
                starting_cells++;
  
            space[i][j] = cell;
        }
    }
}


void FallingSandHelper::MakeChange(const glm::uvec2& from, const glm::uvec2& to)
{
    space_changes.emplace_back(to, from);
}

void FallingSandHelper::CommitChanges()
{
    size_t change_count = space_changes.size();
    for (size_t i = 0; i < change_count; i++)
    {
        if (GetCellAt(space_changes.at(i).first) != _EMPTY)
        {
            space_changes[i] = space_changes.back(); 
            space_changes.pop_back();
            change_count = space_changes.size();
            i--;
        }
    }
    unsigned int temp_rows = rows;
    std::sort(space_changes.begin(), space_changes.end(), [&temp_rows](std::pair<glm::uvec2, glm::uvec2>& a, std::pair<glm::uvec2, glm::uvec2>& b) {return (a.first.x + (a.first.y * temp_rows)) < (b.first.x + (b.first.y * temp_rows)); });
    for (size_t i = 0; i < change_count; i++)
    {
        glm::uvec2 dst = space_changes[i].first;
        glm::uvec2 src = space_changes[i].second;
        
        space[dst.y][dst.x] = space[src.y][src.x];
        space[src.y][src.x] = space[dst.y][dst.x];
        std::cout << "x: " << space_changes.at(i).first.x << " y: " << space_changes.at(i).first.y << " x: " << space_changes.at(i).second.x << " y: " << space_changes.at(i).second.y << std::endl;

    }

    space_changes.clear();
}

unsigned int FallingSandHelper::GetCellAt(const glm::uvec2& pos)
{
    return space[pos.x][pos.y];
}

#include "FallingSandHelper.h"

FallingSandHelper::FallingSandHelper()
{
    size = size_min;
    AllocateEmptySpace(size);
}

FallingSandHelper::~FallingSandHelper()
{
    Deallocate2DSpace();
}

void FallingSandHelper::InitializeSpace(const unsigned int& size)
{
    if (size < size_min)
    {
        std::cout << "Couldn't create simulation space. Defined dimensions are too small" << std::endl;
        return;
    }
    Deallocate2DSpace();
    AllocateEmptySpace(size);
}

void FallingSandHelper::AllocateEmptySpace(const int& new_size)
{
    starting_cells = 0;
    // set the random seed
    std::srand(std::time(nullptr));

    size = new_size;
    size_sq = new_size * new_size;
    size_total = size_sq * new_size;
    space = new unsigned char [size_total];
    std::cout << "Generating " << size*size*size << " number of cells." << std::endl;
    for (unsigned int i = 0; i < size_total; i++) {
        
        int random_number = rand() % 2 + 1;
        space[i] = random_number;
        /*if (i == 5 || i == 9)
        {
            space[i] = _SAND;
        }
        else
            space[i] = _EMPTY;
    */
    }
}

void FallingSandHelper::Deallocate2DSpace()
{
    delete[] space;
}

void FallingSandHelper::IterateSpace()
{
    for (unsigned int  i = 0; i < size_total; i++) {
        
        unsigned char cell = space[i];
            
        GetNeighboarhood(i);    
    }
    //commit changes
    CommitChanges();
}

void FallingSandHelper::IterateThroughSpace(std::function<void(glm::uvec3, unsigned char)> renderFunction)
{
    for (int i = 0; i < size_total; i++) {
            unsigned char cell = space[i];

       renderFunction(ConvertIndexToVec3(i), cell);
    }
}

unsigned int FallingSandHelper::GetSpaceSize() const
{
    return size;
}


void FallingSandHelper::GetNeighboarhood(unsigned int& index)
{
    switch (space[index])
    {
    case _SAND:
    {   
        //check bot
        unsigned int check_index = index + size_sq;
        //check if in bounds
        if (check_index > size_total)
            return;
            
        if (space[index] > space[check_index])
        {
            MakeChange(index, check_index);
            //std::cout << "FROM: " << index << "TO: " << check_index << std::endl;
            return;
        }
                
        
        size_t rand = std::rand() % 4;
        for (size_t i = 1; i <= 4; i++)
        {
            size_t rule = rand + i % 4;
            if (rule == 0) // front
            {
                if (check_index % size != size - 1)
                {
                    if (space[index] > space[check_index + 1])
                    {
                        MakeChange(index, check_index + 1);
                        return;
                    }
                }
                continue;
            }
            if (rule == 1) // back (P.S. we don't check zero because check_index will never be zero when checking for sand rules)
            {
                if (check_index % size != 0)
                {
                    if (space[index] > space[check_index - 1])
                    {
                        MakeChange(index, check_index - 1);
                        return;
                    }
                }
                continue;
            }
            if (rule == 2) // left 
            {
                unsigned int left_index = check_index - size;
                if (check_index / size_sq == left_index / size_sq)
                {
                    if (space[index] > space[left_index])
                    {
                        MakeChange(index, left_index);
                        return;
                    }
                }
                continue;
            }
            if (rule == 3) // right 
            {
                unsigned int right_index = check_index + size;
                if(right_index >= size_total)
                    continue;

                if (check_index / size_sq == right_index / size_sq)
                {
                    if (space[index] > space[right_index])
                    {
                        MakeChange(index, right_index);
                        return;
                    }
                }

                continue;
            }

        }
    }
    case _WATER:
    {
        /*
        if (y + 1 < rows)
        {
            if (space[y + 1][x] == _EMPTY)
            {
                return _BOT_MID;
            }   
        }
        int rand = std::rand() % 2;
        rand = (rand == 0) ? -1 : 1;
        if (x - rand >= 0 && space[y][x - rand] == _EMPTY)
        {
            if (rand == 1)
                return _MID_LEFT;
            
            return _MID_RIGHT;
        }
        if (x + rand < cols && space[y][x + rand] == _EMPTY)
        {
            if(rand==1)
                return _MID_RIGHT;
            
            return _MID_LEFT;
        }
        return _STAY;
        */
    }
        default:
            return;
    }
}

unsigned int FallingSandHelper::GetStartingCells() const
{
    return starting_cells;
}

void FallingSandHelper::SetPixel(const glm::uvec3& pos, const unsigned int& type)
{
    space[ConvertVec3ToIndex(pos)] = type;
}
//might break because uvec are 4 byte and return is 8 byte
size_t FallingSandHelper::ConvertVec3ToIndex(const glm::uvec3& pos) {
    return pos.x + (pos.y * size_sq) + (pos.z * size);
}

//might break because uvec are 4 byte and return is 8 byte
glm::uvec3 FallingSandHelper::ConvertIndexToVec3(const size_t& index) {
    int y = index / size_sq;
    int y_sub = (y * size_sq);
    int z = (index - y_sub) / size;
    int x = (index - (y_sub)-(z * size));
    return glm::uvec3(x, y, z);
}
void FallingSandHelper::MakeChange(const size_t& from, const size_t& to)
{
    space_changes.emplace_back(to, from);
}

void FallingSandHelper::CommitChanges()
{
    size_t change_count = space_changes.size();
    for (size_t i = 0; i < change_count; i++)
    {
        if (space[space_changes.at(i).first] >= space[space_changes.at(i).second])
        {
            space_changes[i] = space_changes.back(); 
            space_changes.pop_back();
            change_count = space_changes.size();
            i--;
        }
    }
   
    std::sort(space_changes.begin(), space_changes.end(), 
        [](std::pair<size_t, size_t>& a, std::pair<size_t, size_t>& b) {return a.first < b.first; });
    for (size_t i = 0; i < change_count; i++)
    {

        unsigned char temp = space[space_changes[i].second];
        space[space_changes[i].second] = space[space_changes[i].first];
        space[space_changes[i].first] = temp;
    }

    space_changes.clear();
}

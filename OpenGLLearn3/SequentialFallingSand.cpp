#include "SequentialFallingSand.h"

SequentialFallingSand::SequentialFallingSand()
{
    space = new unsigned char();
    cubeShader = new Shader("src/res/shaders/fallingSandVertex.glsl", "src/res/shaders/fallingSandFrag.glsl");
    geometry = new MeshGeometry();
    
}

SequentialFallingSand::~SequentialFallingSand()
{
    cubeShader->~Shader();
    geometry->~MeshGeometry();
    DeallocateSpace();
}

void SequentialFallingSand::InitializeSpace(const unsigned int& size, const bool& random) 
{
    unsigned int use_size = size;
    if (size < size_min)
    {
        std::cout << "Couldn't create simulation space. Defined dimensions are too small. Creating minimum size space." << std::endl;
        use_size = size_min;
    }
    lightPos = glm::vec3((float)use_size / 2, (float)use_size, (float)use_size);
    DeallocateSpace();
    CreateSpace(use_size, random);
}

void SequentialFallingSand::CreateSpace(const int& new_size, const bool& random)
{
    starting_cells = 0;
    // set the random seed
    std::srand(std::time(nullptr));

    size = new_size;
    size_sq = new_size * new_size;
    size_total = size_sq * new_size;
    space = new unsigned char [size_total];
    printf("\nGenerating %d number of cells.\n", size * size * size);
    for (unsigned int i = 0; i < size_total; i++) {
        if (random)
        {
            int random_number = rand() % 2;
            
            space[i] = random_number? 3 : 1;
        }
        else
        {
            space[i] = _EMPTY;
        }
    }
}

void SequentialFallingSand::DeallocateSpace()
{
    delete[] space;
}

void SequentialFallingSand::IterateSpace()
{
    cur_cells = 0;
    for (unsigned int  i = 0; i < size_total; i++) {
        if (space[i] > _EMPTY)
        {
            GetNeighboarhood(i);    
            cur_cells++;
        }
    }
    //commit changes
    CommitChanges();
}

void SequentialFallingSand::IterateThroughSpace()
{
    for (int i = 0; i < size_total; i++) {
        unsigned char cell = space[i];
        
        if (cell > _EMPTY)
        {
            if (cell == 1)
                return;
            glm::vec3 color = glm::vec3();

            if (cell == 2)
                color = glm::vec3(0.49f, 0.54f, 0.77f);
            else
                color = glm::vec3(0.98f, 0.8f, 0.35f);
            
            cubeShader->setUniform3f("color", color);
            

            //we can have model matrix be calculated once in exchange for memory
            glm::vec3 pos = ConvertIndexToVec3(i);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3((float)pos.x, -(float)pos.y + size, pos.z));
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
            cubeShader->setUniform4m("model", model);
            geometry->drawCubeManual();
        }
    } 
}


void SequentialFallingSand::GetNeighboarhood(unsigned int& index)
{
    //printf("index: %d value: %d\n", index, space[index]);
    if (space[index] == _SAND)
    {
        //printf("sand: %d\n", index, space[index]);
        if (index + size_sq >= size_total)
            return;

        if (CheckBelow(index))
            return;

        size_t rand = std::rand() % 4;
        unsigned int below = index + size_sq;
        for (size_t i = 1; i <= 4; i++)
        {
            size_t rule = (rand + i) % 4;
            if (rule == 0) // front
            {
                CheckFront(below, index);
                continue;
            }
            if (rule == 1) // back (P.S. we don't check zero because check_index will never be zero when checking for sand rules)
            {
                CheckBack(below, index);
                continue;
            }
            if (rule == 2) // left
            {
                CheckLeft(below, index);
                continue;
            }
            if (rule == 3) // right
            {
                CheckRight(below, index);
                continue;
            }
        }
    }
    else if (space[index] == _WATER)
    {
        //printf("water: %d\n", index, space[index]);
        if (index + size_sq < size_total)
        {
            if (CheckBelow(index))
                return;
        }

        size_t rand = std::rand() % 4;
        
        for (size_t i = 1; i <= 4; i++)
        {
            size_t rule = (rand + i) % 4;

            if (rule == 0) // front
            {
                CheckFront(index, index);
                continue;
            }
            if (rule == 1) // back (P.S. we don't check zero because check_index will never be zero when checking for sand rules)
            {
                CheckBack(index, index);
                continue;
            }
            if (rule == 2) // left
            {
                CheckLeft(index, index);
                continue;
            }
            if (rule == 3) // right
            {
                CheckRight(index, index);
                continue;
            }
        }
    }
}
#pragma region Checks
bool SequentialFallingSand::CheckBelow(const size_t& index)
{
    //check bot
    unsigned int check_index = index + size_sq;

    if (space[index] > space[check_index])
    {
        MakeChange(index, check_index);
        return true;
    }
    return false;
}
void SequentialFallingSand::CheckFront(const size_t& index, const size_t& swap)
{
    if (index % size == size - 1)
        return;
    size_t check = index + 1;
    if (space[swap] > space[check])
        MakeChange(swap, check);
}
void SequentialFallingSand::CheckBack(const size_t& index, const size_t& swap)
{
    if (index % size == 0)
        return;

    size_t check = index - 1;
    if (space[index] > space[check])
        MakeChange(swap, check);
}

void SequentialFallingSand::CheckLeft(const size_t& index, const size_t& swap)
{
    unsigned int left_index = index - size;
    if (index / size_sq == left_index / size_sq)
    {
        if (space[index] > space[left_index])
        {
            MakeChange(swap, left_index);
            return;
        }
    }
}
void SequentialFallingSand::CheckRight(const size_t& index, const size_t& swap)
{
    unsigned int right_index = index + size;
    if (right_index >= size_total)
        return;

    if (index / size_sq == right_index / size_sq)
    {
        if (space[index] > space[right_index])
        {
            MakeChange(swap, right_index);
            return;
        }
    }
}
#pragma endregion

void SequentialFallingSand::SetPixel(const glm::uvec3& pos, const unsigned int& type)
{
    space[ConvertVec3ToIndex(pos)] = type;
}

unsigned int SequentialFallingSand::ClampInSpace(unsigned int value)
{
    
    return std::max(0, (int)std::min(value, size-1));
}

//might break because uvec are 4 byte and return is 8 byte
size_t SequentialFallingSand::ConvertVec3ToIndex(const glm::uvec3& pos) {
    
    return ClampInSpace(pos.x) + (ClampInSpace(pos.y) * size_sq) + (ClampInSpace(pos.z) * size);
}

//might break because uvec are 4 byte and return is 8 byte
glm::uvec3 SequentialFallingSand::ConvertIndexToVec3(const size_t& index) {
    int y = index / size_sq;
    int y_sub = (y * size_sq);
    int z = (index - y_sub) / size;
    int x = (index - (y_sub)-(z * size));
    return glm::uvec3(x, y, z);
}
void SequentialFallingSand::MakeChange(const size_t& from, const size_t& to)
{
    space_changes.emplace_back(to, from);
}

void SequentialFallingSand::CommitChanges()
{
    size_t change_count = space_changes.size();
    //this part is not needed
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
   
   // std::sort(space_changes.begin(), space_changes.end(), 
   //     [](std::pair<size_t, size_t>& a, std::pair<size_t, size_t>& b) {return a.first < b.first; });
    for (size_t i = 0; i < change_count; i++)
    {
        //printf("FROM: %d TO: %d\n", space_changes[i].second, space_changes[i].first);
        unsigned char temp = space[space_changes[i].second];
        space[space_changes[i].second] = space[space_changes[i].first];
        space[space_changes[i].first] = temp;
    }

    space_changes.clear();
}
unsigned int SequentialFallingSand::GetCellCount() const
{
    return cur_cells;
}

void SequentialFallingSand::RenderSpace(const glm::mat4& view, const glm::mat4& projection)
{
    geometry->bindCube();
    cubeShader->use();
    cubeShader->setUniform3f("lightPos", lightPos);
    cubeShader->setUniform4m("projection", projection);
    cubeShader->setUniform4m("view", view);

    IterateThroughSpace();
}
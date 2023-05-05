#include "ParallelFallingSand.h"

ParallelFallingSand::ParallelFallingSand()
{
    determineShader = new Shader("src/res/shaders/compute/computeSand.glsl");
    swapShader      = new Shader("src/res/shaders/compute/computeSwaps.glsl");
    renderShader    = new Shader("src/res/shaders/compute/computeVert.glsl", "src/res/shaders/compute/computeFrag.glsl");
    posSSBO = new unsigned int();
    swapSSBO = new unsigned int();
    vao = new unsigned int();
}

ParallelFallingSand::~ParallelFallingSand()
{

}
void ParallelFallingSand::DeallocateSpace()
{
    determineShader->~Shader();
    swapShader->~Shader();
    renderShader->~Shader();
    glDeleteBuffers(1, posSSBO);
    glDeleteBuffers(1, swapSSBO);
}
void ParallelFallingSand::InitializeSpace(const unsigned int& size, const bool& random)
{
    unsigned int use_size = size;
    if (size < size_min)
    {
        printf("\nCouldn't create simulation space. Defined dimensions are too small. Creating minimum size space.\n");
        use_size = size_min;
    }
#pragma region Cells buffer
    this->size = use_size;
    size_sq = use_size * use_size;
    size_total = size_sq * use_size;
    printf("\nGenerating %d sized buffer of struct pos (3 floats and 1 int)\n", size_total);
    struct swaps {
        GLint from;
        GLint to;
    };
    struct pos
    {
        GLfloat x, y, z; // positions
        GLint state;
    };
    glGenBuffers(1, posSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, *posSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size_total * sizeof(struct pos), NULL, GL_STATIC_DRAW);
    GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
    struct pos* points = (struct pos*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size_total* (sizeof(GLfloat)*3+sizeof(GLint)), bufMask);
    int counter = 0;
    for (int i = 0; i < use_size; i++)
    {
        for (int j = 0; j < use_size; j++)
        {
            for (int k = 0; k < use_size; k++)
            {
                points[counter].x = j;
                points[counter].y = use_size - 1 - i;
                points[counter].z = use_size - 1 - k;
                if (random)
                    points[counter].state = rand() % 2;
                else if (counter == 26 - (use_size * 2) || counter == 25 - (use_size * 2) || counter == 17 - (use_size * 2))
                {
                    points[counter].state = 1;
                }
                else
                    points[counter].state = 0;
                //printf("index %d: x: %d, y: %d, z: %d\n", counter, j, i, k);
                counter++;
            }
        }
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, *posSSBO);

    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, *posSSBO);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
#pragma endregion 
#pragma region swapsBuffer
    printf("\nGenerating %d sized buffer of struct swap (2 uint)\n", size_total);

    glGenBuffers(1, swapSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, *swapSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size_total * sizeof(GLint) * 2, NULL, GL_STATIC_DRAW);

    struct swaps* swaps = (struct swaps*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size_total * sizeof(GLint)*2, bufMask);
    counter = 0;
    for (int i = 0; i < use_size; i++)
    {
        for (int j = 0; j < use_size; j++)
        {
            for (int k = 0; k < use_size; k++)
            {
                swaps[counter].from = -1;
                swaps[counter].to = -1;
                counter++;
            }
        }
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, *swapSSBO);

    glEnableVertexAttribArray(5);
    glBindBuffer(GL_ARRAY_BUFFER, *swapSSBO);
    glVertexAttribPointer(5, 2, GL_INT, GL_FALSE, 0, (void *)0);

    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);
#pragma endregion
   
}

float ParallelFallingSand::IterateSpace()
{
    GLint gpuTime;
    GLuint query;
    glGenQueries(1, &query);
    glBeginQuery(GL_TIME_ELAPSED, query);

    determineShader->use();
    determineShader->setUniform1uint("size", size);
    determineShader->setUniform1uint("size_sq", size_sq);
    determineShader->setUniform1uint("volume", size_total);
    glDispatchCompute(size_total, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    swapShader->use();
    glDispatchCompute(size_total, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glEndQuery(GL_TIME_ELAPSED);
    glGetQueryObjectiv(query, GL_QUERY_RESULT, &gpuTime);
    return (float)gpuTime / 1000000.0f;
}

unsigned int ParallelFallingSand::GetCellCount() const
{
	return 0;
}

void ParallelFallingSand::RenderSpace(const glm::mat4& view, const glm::mat4& projection)
{
    renderShader->use();
    renderShader->setUniform4m("projection", projection);
    renderShader->setUniform4m("view", view);
    
    glDrawArrays(GL_POINTS, 0, size_total);
}

#include "ParallelFallingSand.h"

void ParallelFallingSand::InitializeSpace(const unsigned int& size, const bool& random)
{
}

void ParallelFallingSand::IterateSpace()
{
}

unsigned int ParallelFallingSand::GetCellCount() const
{
	return 0;
}

void ParallelFallingSand::DeallocateSpace()
{
	glDeleteBuffers(1, &posSSBO);
	glDeleteBuffers(1, &swapSSBO);
}
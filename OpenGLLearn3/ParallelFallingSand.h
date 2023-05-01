#pragma once
#include "IFallingSand.h"
#include <glad/glad.h>
class ParallelFallingSand : public IFallingSand
{
public:
	void InitializeSpace(const unsigned int& size, const bool& random) override;
	void IterateSpace() override;
	unsigned int GetCellCount() const override;
	void RenderSpace();
private:
	void virtual DeallocateSpace() override;
	
	unsigned int posSSBO;
	unsigned int swapSSBO;
};
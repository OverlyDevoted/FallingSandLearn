#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include "IFallingSand.h"
#include "ParallelFallingSand.h"
#include "SequentialFallingSand.h"

enum SandType {
	_SEQUENTIAL_SAND,
	_PARALLEL_SAND
};
class SimulationHelper
{
public:
	SimulationHelper();
	~SimulationHelper();
	void InitializeSpace(SandType sim_type, const unsigned int& size, const bool& random);
	void RenderSpace(const glm::mat4& view, const glm::mat4& projection);
	float IterateSpace();
	unsigned int GetSpaceSize() const;
	unsigned int GetStartingCells() const;
	void DeallocateSpace();
private:
	IFallingSand* sand;
};

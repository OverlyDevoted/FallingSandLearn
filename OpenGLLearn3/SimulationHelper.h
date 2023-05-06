#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include "IFallingSand.h"
#include "ParallelFallingSand.h"
#include "SequentialFallingSand.h"
#include <utility>
enum class SandType : int {
	_SEQUENTIAL_SAND = 0,
	_PARALLEL_SAND = 1
};
class SimulationHelper
{
public:
	SimulationHelper();
	~SimulationHelper();
	std::pair<int, int> InitializeSpace(SandType sim_type, const unsigned int& size, const bool& random);
	void RenderSpace(const glm::mat4& view, const glm::mat4& projection);
	float IterateSpace();
	unsigned int GetSpaceSize() const;
	unsigned int GetStartingCells() const;
	void DeallocateSpace();
	unsigned int GetSizeMin() const;
private:
	IFallingSand* sand;
};

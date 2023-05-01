#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include "IFallingSand.h"
#include "ParallelFallingSand.h"
#include "SequentialFallingSand.h"

class SimulationHelper
{
public:
	SimulationHelper();
	~SimulationHelper();
	void InitializeSpace(const unsigned int& size, const bool& random);
	void RenderSpace(const glm::mat4& view, const glm::mat4& projection);
	void IterateSpace();
	unsigned int GetSpaceSize() const;
	unsigned int GetStartingCells() const;

private:
	IFallingSand* sand;
};

enum SimType {
	_SEQUENTIAL,
	_PARALLEL
};
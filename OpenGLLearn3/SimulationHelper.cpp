#include "SimulationHelper.h"

SimulationHelper::SimulationHelper()
{
}

SimulationHelper::~SimulationHelper()
{
	delete sand;
}

void SimulationHelper::InitializeSpace(const unsigned int& size, const bool& random)
{
	sand = new SequentialFallingSand();
	sand->InitializeSpace(size, random);
}

void SimulationHelper::RenderSpace(const glm::mat4 &view, const glm::mat4 &projection)
{
	sand->RenderSpace(view, projection);
}

void SimulationHelper::IterateSpace()
{
	sand->IterateSpace();
}


unsigned int SimulationHelper::GetSpaceSize() const
{
	return sand->GetSpaceSize();
}

unsigned int SimulationHelper::GetStartingCells() const
{
	return sand->GetStartingCells();
}

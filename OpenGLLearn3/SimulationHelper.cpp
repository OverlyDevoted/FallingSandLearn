#include "SimulationHelper.h"

SimulationHelper::SimulationHelper()
{
}

SimulationHelper::~SimulationHelper()
{
	delete sand;
}

void SimulationHelper::InitializeSpace(SandType sim_type, const unsigned int& size, const bool& random)
{
	if(sim_type == _PARALLEL_SAND)
		sand = new ParallelFallingSand();
	else
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

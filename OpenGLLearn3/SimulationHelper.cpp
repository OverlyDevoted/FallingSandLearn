#include "SimulationHelper.h"

SimulationHelper::SimulationHelper()
{
}

SimulationHelper::~SimulationHelper()
{
	delete sand;
}

std::pair<int, int> SimulationHelper::InitializeSpace(SandType sim_type, const unsigned int& size, const bool& random)
{
	if(sim_type == SandType::_PARALLEL_SAND)
		sand = new ParallelFallingSand();
	else
		sand = new SequentialFallingSand();
	return sand->InitializeSpace(size, random);
}
void SimulationHelper::DeallocateSpace()
{
	sand->DeallocateSpace();
}
void SimulationHelper::RenderSpace(const glm::mat4 &view, const glm::mat4 &projection)
{
	sand->RenderSpace(view, projection);
}

float SimulationHelper::IterateSpace()
{
	return sand->IterateSpace();
}


unsigned int SimulationHelper::GetSpaceSize() const
{
	return sand->GetSpaceSize();
}

unsigned int SimulationHelper::GetStartingCells() const
{
	return sand->GetStartingCells();
}

unsigned int SimulationHelper::GetSizeMin() const
{
	return sand->GetSizeMin();
}
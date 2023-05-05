#pragma once
#include <glm/glm.hpp>
class IFallingSand
{
public:
	virtual ~IFallingSand() {};
	virtual void InitializeSpace(const unsigned int& size, const bool& random) = 0;
	virtual float IterateSpace() = 0;
	virtual unsigned int GetSpaceSize() const { return size; };
	virtual unsigned int GetStartingCells() const { return starting_cells; };
	virtual unsigned int GetCellCount() const = 0;
	virtual unsigned int GetSizeMin() const { return size_min; };
	virtual void RenderSpace(const glm::mat4& view, const glm::mat4& projection) = 0;
	virtual void DeallocateSpace() = 0;
protected:
	unsigned int size_min = 3;
	unsigned int size;
	unsigned int size_sq;
	unsigned int size_total;
	unsigned int starting_cells = 0;
private:
};

enum CellType {
	_VOID = 0,
	_EMPTY = 1,
	_WATER = 2,
	_SAND = 3,
};
enum CellMove {
	_STAY = 0,
	_BOT_MID = 1,
	_BOT_LEFT = 2,
	_BOT_RIGHT = 3,
	_BOT_FORW = 4,
	_BOT_BACK = 5
};
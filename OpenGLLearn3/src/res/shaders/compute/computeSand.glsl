#version 430 core

struct cell {
	vec3 pos;
	int state;
};
layout(std430, binding = 4) buffer Pos
{
	cell state[];
};

layout(std430, binding = 5) buffer Swaps
{
	ivec2 swap[];
};
layout(local_size_x = 512, local_size_y = 1, local_size_z = 1) in; 
uniform uint size;
uniform uint size_sq;
uniform uint volume;

bool checkBelow(uint gid, uint below)
{
	return state[gid].state > state[below].state;
}
bool checkFront(uint gid, uint below)
{
	if (below % size == size - 1)
		return false;
	uint check = below + 1;
	return state[gid].state > state[check].state;
}
bool checkBack(uint gid, uint below)
{
	if (below % size == 0)
		return false;

	uint check = below - 1;
	return state[gid].state > state[check].state;
}
bool checkLeft(uint gid, uint below)
{
	uint left_index = below - size;
	if (below / size_sq == left_index / size_sq)
		return state[gid].state > state[left_index].state;
	return false;
}
bool checkRight(uint gid, uint below)
{
	uint right_index = below + size;
	if (right_index >= volume)
		return false;

	if (below / size_sq == right_index / size_sq)
		return state[below].state > state[right_index].state;
	return false;
}
void main()
{
	uint gid = gl_GlobalInvocationID.x; // the cell which is currently invoked

	if (state[gid].state == 0)
		return;

	uint below = gid + size_sq;
	if (below >= volume) // clamp instead of ifs
		return;
	
	if (checkBelow(gid, below))
	{
		swap[below].x = int(below);
		swap[below].y = int(gid);
		return;
	}
	else if(checkFront(gid,below))
	{
		swap[below + 1].x = int(below + 1);
		swap[below + 1].y = int(gid);
		return;
	}
	else if (checkBack(gid, below))
	{
		swap[below - 1].x = int(below - 1);
		swap[below - 1].y = int(gid);
		return;
	}
	else if (checkLeft(gid, below))
	{
		swap[below - size].x = int(below - size);
		swap[below - size].y = int(gid);
	}
	else if (checkRight(gid, below))
	{
		swap[below + size].x = int(below + size);
		swap[below + size].y = int(gid);
	}
}

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
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in; 
uniform uint size;
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

void main()
{
	uint gid = gl_GlobalInvocationID.x;

	if (state[gid].state == 0)
		return;

	uint below = gid + (size * size);
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
		swap[below+1].x = int(below+1);
		swap[below+1].y = int(gid);
		return;
	}
}

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
void main()
{
	uint gid = gl_GlobalInvocationID.x;
	

	swap[gid].x = -1;
	swap[gid].y = -1;

	if (state[gid].state == 0)
		return;

	uint below = gid + (size * size);
	if (below < volume) // clamp instead of ifs
	{
		if (state[gid].state > state[below].state)
		{
			swap[below].x = int(below);
			swap[below].y = int(gid);
		}
	}
}

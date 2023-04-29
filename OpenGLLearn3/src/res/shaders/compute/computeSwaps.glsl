#version 430 core

layout(std430, binding = 4) buffer Pos
{
	vec4 state[];
};

layout(std430, binding = 5) buffer Swaps
{
	ivec2 swap[];
};
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main()
{
	uint gid = gl_GlobalInvocationID.x;
	if (swap[gid].x == -1)
		return;
	uint from	= uint(swap[gid].x);
	uint to		= uint(swap[gid].y);
	float tempFrom = state[from].w;
	state[from].w = state[to].w;
	state[to].w = tempFrom;
}

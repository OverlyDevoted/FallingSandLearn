#version 430 core
struct cell {
    vec3 pos;
    int state;
};
layout(std430, binding = 4) buffer Pos
{
    cell state[];
};

uniform mat4 view;
uniform mat4 projection;

out vec3 fragmentColor;

void main() {
    // Set position accoring to VP matrices
    gl_Position = projection * view * vec4(state[gl_VertexID].pos.xyz, 1.0);
    // Modify particle size to give a sense of depth
    // A realistic model would divide by (z*z), but there are only so many pixels in a screen
    gl_PointSize = 200.0 / gl_Position.z;

    //forward color data on to fragment shader
    if (state[gl_VertexID].state == 1)
        fragmentColor = vec3(1.0, 1.0, 0.0);
    else
        fragmentColor = vec3(0.0, 0.0, 0.0);
}
#version 330 core
layout(location = 0) in vec3 aPos; 
layout(location = 1) in vec3 aCol;
layout(location = 2) in vec2 aTexCoord;

out vec3 vertexColor;
out vec2 TexCoord;

uniform vec4 horizontalOffset;

void main()
{
    gl_Position = vec4(aPos.xyz + horizontalOffset.xyz, 1.0);
    vertexColor = aCol;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
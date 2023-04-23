#version 430 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 color;
uniform vec3 lightPos;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0,1.0,1.0);
    vec3 result = (0.4+diffuse) * color;
    FragColor = vec4(result, 1.0);
}
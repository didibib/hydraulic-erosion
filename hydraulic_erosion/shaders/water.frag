#version 400 core

in vec3 FragPos;
in vec4 Color;
in vec3 Normal;

out vec4 FragColor;

void main()
{
    FragColor = Color;
}

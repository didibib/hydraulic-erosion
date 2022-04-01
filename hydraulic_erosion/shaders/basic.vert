#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;

out vec4 Color;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

void main()
{
    gl_Position = u_ProjectionView * u_Model * vec4(a_Position, 1.0);
    Color = a_Color;
}
#version 400 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_Uv;
layout (location = 3) in vec3 a_Normal;

out vec3 Normal;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;
uniform mat4 u_NormalMatrix;

void main()
{
    Normal = normalize((u_NormalMatrix * vec4(a_Normal, 0.0)).xyz);
    gl_Position = u_ProjectionView * u_Model * vec4(a_Position, 1.0);
}

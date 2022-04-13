#version 400 core

in vec3 Normal;

out vec4 FragColor;

uniform sampler2D u_Texture1;
uniform mat4 u_View;

void main()
{    
    vec2 muv = normalize(Normal).xy * 0.5 + vec2(0.5, 0.5);
    FragColor = texture2D(u_Texture1, vec2(muv.x, muv.y));
}


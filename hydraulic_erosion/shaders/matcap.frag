#version 400 core

in vec3 Normal;

out vec4 FragColor;

uniform vec3 u_CameraPos;
uniform sampler2D u_Texture1;

vec2 matcap(vec3 _cameraPos, vec3 _normal){
    vec3 reflected = reflect(_cameraPos, _normal);
    float m = 2.8284271247461903 * sqrt(reflected.z + 1.0);
    return reflected.xy / m + 0.5;
}

void main()
{
    vec2 uv = matcap(u_CameraPos, Normal);
    FragColor = vec4(texture2D(u_Texture1, uv).rgb, 1.0);
}

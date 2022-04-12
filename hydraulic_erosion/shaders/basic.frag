#version 400 core

in vec3 FragPos;
in vec4 Color;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 u_LightPos;
uniform vec3 u_LightColor;

void main()
{
    // ambient
    float ambientStrength = .5;
    vec3 ambient = ambientStrength * u_LightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;
            
    vec3 result = (ambient + diffuse) * Color.xyz;
    FragColor = vec4(result, 1.0);
}

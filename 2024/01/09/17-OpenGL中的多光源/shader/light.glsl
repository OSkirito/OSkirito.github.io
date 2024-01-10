#version 330 core
out vec4 FragColor;

uniform vec3 lightColor;
uniform bool enable;

void main()
{
    if (enable)
    {
        FragColor = vec4(lightColor, 1.0);
    }
    else
    {
        FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}
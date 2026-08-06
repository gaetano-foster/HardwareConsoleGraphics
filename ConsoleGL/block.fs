#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec2 uv;
in vec3 vertexColor;

uniform sampler2D tex;

void 
main()
{
   FragColor = texture(tex, uv) * 2;
}
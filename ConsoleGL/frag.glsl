#version 330 core

in vec3 normal;
in vec2 uv;
in vec3 vertexColor;

out vec4 FragColor;

void 
main()
{
   FragColor = vec4(vertexColor, 1.0);
   //FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
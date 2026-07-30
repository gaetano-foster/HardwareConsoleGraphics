#version 330 core

layout (location = 0) in vec3 aPos;
out vec3 vertexColor;

uniform mat4 model;
uniform mat4 proj;
uniform mat4 view;

void 
main()
{
	gl_Position = model * vec4(aPos, 1.0);
	vertexColor = aPos + 0.5;
}
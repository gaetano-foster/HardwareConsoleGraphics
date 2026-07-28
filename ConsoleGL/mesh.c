#include <glad/glad.h>
#include <Windows.h>
#include <stdio.h>
#include "mesh.h"

extern const char *VERTEX_SOURCE = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

extern const char *FRAGMENT_SOURCE = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
"}\0";

void
init_mesh_buffers(struct mesh_t *mesh)
{
	// create vbo
	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh->size, mesh->vertices, GL_STATIC_DRAW);
	// create vao
	glGenVertexArrays(1, &mesh->vao); 
	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vao);
	glBufferData(GL_ARRAY_BUFFER, mesh->size, mesh->vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// tell gpu how to interpret vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// copy vertices array into buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh->size, mesh->vertices, GL_STATIC_DRAW);
	// set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	mesh->count = mesh->size / sizeof(float) / 3;
}

void
render_mesh(struct mesh_t mesh)
{
	glBindVertexArray(mesh.vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.count);
}

BOOL
compile_shaders(DWORD *shader_program)
{
	DWORD vertex_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &VERTEX_SOURCE, NULL);
	glCompileShader(vertex_shader);

	DWORD fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &FRAGMENT_SOURCE, NULL);
	glCompileShader(fragment_shader);

	BOOL success;
	char info_log[512];

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s", info_log);
		return FALSE;
	}

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s", info_log);
		return FALSE;
	}

	*shader_program = glCreateProgram();
	glAttachShader(*shader_program, vertex_shader);
	glAttachShader(*shader_program, fragment_shader);
	glLinkProgram(*shader_program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader); 
	return TRUE;
}
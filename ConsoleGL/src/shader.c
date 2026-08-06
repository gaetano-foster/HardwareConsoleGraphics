#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <Windows.h>
#include "shader.h"
#include "utils.h"
#include "camera.h"

BOOL
shader_compile(shader_t *shader,
	const char *vert_source,
	const char *frag_source)
{
	// compile vertex shadder
	GLuint vertex_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vert_source, NULL);
	glCompileShader(vertex_shader);
	// compile fragment shader
	GLuint fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &frag_source, NULL);
	glCompileShader(fragment_shader);
	// make sure vertex and fragment shader compiled successfully
	BOOL success;
	char info_log[512];

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		fprintf(stderr, "Error: Vertex Shader Compilation failed!\n%s", info_log);
		return FALSE;
	}

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		fprintf(stderr, "Error: Fragment Shader Compilation failed!\n%s", info_log);
		return FALSE;
	}
	// link shader program
	shader->id = glCreateProgram();
	glAttachShader(shader->id, vertex_shader);
	glAttachShader(shader->id, fragment_shader);
	glLinkProgram(shader->id);
	// populate uniform values
	EXPECT((shader->model_loc = glGetUniformLocation(shader->id, "model")) != -1);
	EXPECT((shader->proj_loc = glGetUniformLocation(shader->id, "proj")) != -1);
	EXPECT((shader->view_loc = glGetUniformLocation(shader->id, "view")) != -1);
	EXPECT((shader->tex_loc = glGetUniformLocation(shader->id, "tex")) != -1);
	// clean up
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader); 
	return TRUE;
}

void
shader_destroy(shader_t shader)
{
	glDeleteProgram(shader.id);
}

void
shader_use(shader_t shader)
{
	glUseProgram(shader.id);
}
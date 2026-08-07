#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <Windows.h>
#include "shader.h"
#include "utils.h"
#include "camera.h"

shader_t *
shader_compile(const char *vert_source, const char *frag_source)
{
	shader_t *shader;
	if (!(shader = malloc(sizeof(shader_t)))) {
		fprintf(stderr, "Failed to allocate memory for mesh.");
		return NULL;
	}
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
		free(shader);
		return NULL;
	}

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		fprintf(stderr, "Error: Fragment Shader Compilation failed!\n%s", info_log);
		free(shader);
		return NULL;
	}
	// link shader program
	shader->id = glCreateProgram();
	glAttachShader(shader->id, vertex_shader);
	glAttachShader(shader->id, fragment_shader);
	glLinkProgram(shader->id);
	// populate uniform values
	if (!((shader->model_loc = glGetUniformLocation(shader->id, "model")) != -1)
		|| (!((shader->proj_loc = glGetUniformLocation(shader->id, "proj")) != -1))
		|| (!((shader->view_loc = glGetUniformLocation(shader->id, "view")) != -1))
		|| (!((shader->tex_loc = glGetUniformLocation(shader->id, "tex")) != -1))) {
		fprintf(stderr, "Error: Uniform variable not found!\n");
		free(shader);
		return NULL;
	}
	// clean up
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader); 
	return shader;
}

void
shader_cleanup(shader_t *shader)
{
	glDeleteProgram(shader->id);
	free(shader);
}

void
shader_use(shader_t *shader)
{
	glUseProgram(shader->id);
}
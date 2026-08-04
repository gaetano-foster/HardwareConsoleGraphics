#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <Windows.h>
#include "shader.h"
#include "utils.h"
#include "camera.h"

// Returns null terminated, heap allocated string that must be freed by the caller
static char *
read_shader(const char *path)
{
	FILE *file;
	if (!(file= fopen(path, "r"))) {
		fprintf(stderr, "");
		return FALSE;
	}

	if (fseek(file, 0, SEEK_END) != 0) {
		perror("Error seeking file");
		fclose(file);
		return NULL;
	}

	long file_size = ftell(file);
	if (file_size < 0) {
		perror("Error determining file size");
		fclose(file);
		return NULL;
	}	
	rewind(file);

	char* buffer = (char*)malloc(file_size + 1);
	if (buffer == NULL) {
		perror("Memory allocation failed");
		fclose(file);
		return NULL;
	}

	size_t bytes_read = fread(buffer, 1, file_size, file);
	if (bytes_read < (size_t)file_size) {
		if (ferror(file)) {
			perror("Error reading file");
			free(buffer);
			fclose(file);
			return NULL;
		}
		file_size = bytes_read; 
	}

	buffer[file_size] = '\0';

	fclose(file);
	return buffer;
}

BOOL
shader_compile(shader_t *shader,
	const char *vert_path,
	const char *frag_path)
{
	// read vertex and fragment shader source code
	char *vertex_source, *fragment_source;
	EXPECT((vertex_source = read_shader(vert_path)) != NULL);
	EXPECT((fragment_source = read_shader(frag_path)) != NULL);
	// compile vertex shadder
	GLuint vertex_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_source, NULL);
	glCompileShader(vertex_shader);
	// compile fragment shader
	GLuint fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_source, NULL);
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
	// clean up
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader); 
	free(vertex_source);
	free(fragment_source);
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
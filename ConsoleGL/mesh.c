#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#include <Windows.h>
#include <stdio.h>
#include "mesh.h"
#include "expect.h"

void
init_mesh_buffers(struct mesh_t *mesh)
{
	// create vao and vbo
	glGenVertexArrays(1, &mesh->vao);
	glGenBuffers(1, &mesh->vbo);
	// bind vao
	glBindVertexArray(mesh->vao);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER,
		mesh->size,
		mesh->vertices,
		GL_STATIC_DRAW);

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(float),
		(void *)0);
	glEnableVertexAttribArray(0);

	mesh->count = mesh->size / (3 * sizeof(float));
}

void
render_mesh(struct mesh_t mesh)
{
	glBindVertexArray(mesh.vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.count);
}

char *
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
compile_shaders(GLuint *shader_program)
{
	char *vertex_source, *fragment_source;
	EXPECT((vertex_source = read_shader("vert.glsl")) != NULL);
	EXPECT((fragment_source = read_shader("frag.glsl")) != NULL);

	GLuint vertex_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_source, NULL);
	glCompileShader(vertex_shader);

	GLuint fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_source, NULL);
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
	free(vertex_source);
	free(fragment_source);
	return TRUE;
}
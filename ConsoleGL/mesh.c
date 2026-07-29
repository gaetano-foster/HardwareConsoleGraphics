#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#include <Windows.h>
#include <stdio.h>
#include "mesh.h"
#include "expect.h"

void
mesh_init_buffers(mesh_t *mesh)
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
mesh_draw(mesh_t mesh)
{
	glBindVertexArray(mesh.vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.count);
}

void 
mesh_destroy(mesh_t mesh)
{
	glDeleteVertexArrays(1, &mesh.vao);
	glDeleteBuffers(1, &mesh.vbo);
}
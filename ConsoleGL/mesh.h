#ifndef __MESH_H__
#define __MESH_H__

typedef struct _mesh_t {
	GLuint vao;
	GLuint vbo;
	const float *vertices;
	GLsizei size;
	GLsizei count;
} mesh_t;

void
mesh_init_buffers(mesh_t *mesh);

void
mesh_draw(mesh_t mesh);

#endif // !__MESH_H__

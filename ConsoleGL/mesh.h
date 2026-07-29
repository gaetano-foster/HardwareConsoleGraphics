#ifndef __MESH_H__
#define __MESH_H__

struct mesh_t {
	GLuint vao;
	GLuint vbo;
	const float *vertices;
	GLsizei size;
	GLsizei count;
};

void
mesh_init_buffers(struct mesh_t *mesh);

void
mesh_draw(struct mesh_t mesh);

#endif // !__MESH_H__

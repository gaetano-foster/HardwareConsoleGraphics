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
init_mesh_buffers(struct mesh_t *mesh);

void
render_mesh(struct mesh_t mesh);

BOOL
compile_shaders(GLuint *shader_program);

#endif // !__MESH_H__

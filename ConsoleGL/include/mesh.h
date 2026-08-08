#ifndef __MESH_H__
#define __MESH_H__
#include <glad/glad.h>
#include <cglm/cglm.h>

/*
Structure for storing Mesh data

Fields:
	- vao: Vertex Array Object ID used by OpenGL functions
	- vbo: Vertex Buffer Object ID used by OpenGL functions 
	- ebo: Element Buffer Object ID used by OpenGL functions 
	- element_count: Number of indices
*/
typedef struct _mesh_t {
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLsizei element_count;
} mesh_t;

/*
Structure for storing a vertex, including positional data,
normal data, and UV coordinates
*/
typedef struct _vertex_t {
	vec3 position;
	vec3 normal;
	vec2 uv;
} vertex_t;

/*
Returns heap allocated pointer to mesh read in from file at path
that must be freed with mesh_cleanup
*/
mesh_t *
mesh_load(const char *path);

/*
Frees memory associated with mesh
*/
void
mesh_cleanup(mesh_t *mesh);

/*
Draws the mesh to the OpenGL frame buffer
*/
void
mesh_draw(mesh_t *mesh);

#endif // !__MESH_H__

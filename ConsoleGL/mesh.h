#ifndef __MESH_H__
#define __MESH_H__

/*
Structure for storing Mesh data.

Fields:
	- vao - Vertex Array Object ID used by OpenGL functions
	- vbo - Vertex Buffer Object ID used by OpenGL functions 
	- vertices - array of floats that represents the physical vertices
	- size - total size of the vertices array
	- count - number of vertices

TODO:
	- Vertices, size, and count don't need to be stored:
	  Refactor to be a simple wrapper around VAO and VBO
*/
typedef struct _mesh_t {
	GLuint vao;
	GLuint vbo;
	const float *vertices;
	GLsizei size;
	GLsizei count;
} mesh_t;

/*
Initializes the VAO, VBO, and count of the given mesh.

Parameters:
	- mesh_t *mesh: pointer to previously allocated mesh with caller 
	  defined vertices and size

TODO:
	- Turn this into a function that loads a mesh from a file
*/
void
mesh_init_buffers(mesh_t *mesh);

/*
Frees memory associated with mesh
*/
void
mesh_destroy(mesh_t *mesh);

/*
Draws the mesh to the OpenGL frame buffer
*/
void
mesh_draw(mesh_t *mesh);

#endif // !__MESH_H__

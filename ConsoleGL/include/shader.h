#ifndef __SHADER_H__
#define __SHADER_H__
#include <glad/glad.h>

/*
Represents a shader program
*/
typedef struct _shader_t {
	GLuint id;
	// cached uniform locations
	GLint model_loc;
	GLint proj_loc;
	GLint view_loc;
	GLint tex_loc;
} shader_t;

/*
Returns heap allocated pointer to shader compiled
that must be freed with shader_cleanup

Parameters:
	- const char *vert_source: source code for vertex shader
	- const char *frag_source: source code for fragment shader
*/
shader_t *
shader_compile(const char *vert_source, const char *frag_source);

/*
Frees memory associated with shader
*/
void
shader_cleanup(shader_t *shader);

/*
Tells OpenGL context to use shader
*/
void
shader_use(shader_t *shader);

#endif

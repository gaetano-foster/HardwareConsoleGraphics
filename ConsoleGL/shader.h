#ifndef __SHADER_H__
#define __SHADER_H__

/*
Represents a shader program
*/
typedef struct _shader_t {
	GLuint id;
	// cached uniform locations
	GLint model_loc;
	GLint proj_loc;
	GLint view_loc;
} shader_t;

/*
Compiles a shader program

Parameters:
	- shader_t *shader: shader program structure to be populated
	- const char *vert_path: path to vertex shader file
	- const char *frag_path: path to fragment shader file

Return Values:
	- FALSE (0): Indicates failure
	- TRUE	(1): Indicates success
*/
BOOL
shader_compile(shader_t *shader,
	const char *vert_path, 
	const char *frag_path);

/*
Frees memory associated with shader
*/
void
shader_destroy(shader_t shader);

/*
Tells OpenGL context to use shader
*/
void
shader_use(shader_t shader);

#endif

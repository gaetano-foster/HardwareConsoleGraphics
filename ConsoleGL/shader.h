#ifndef __SHADER_H__
#define __SHADER_H__

/*
Represents a Shader Program ID
*/
typedef GLuint shaderprog_t;

/*
Compiles a Shader Program

Parameters:
	- shaderprog_t *shader:  output pointer for shader program
	- const char *vert_path: path to vertex shader file
	- const char *frag_path: path to fragment shader file

Return Values:
	- FALSE (0): Indicates failure
	- TRUE	(1): Indicates success
*/
BOOL
shaderprog_compile(shaderprog_t *shader,
	const char *vert_path, 
	const char *frag_path);

#endif

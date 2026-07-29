#ifndef __SHADER_H__
#define __SHADER_H__

typedef GLuint shaderprog_t;

BOOL
shaderprog_compile(shaderprog_t *shader,
	const char *vert_path, 
	const char *frag_path);

void
shader_seti(shaderprog_t shader, 
	const char *name, 
	int value);

void
shader_setf(shaderprog_t shader, 
	const char *name, 
	float value);

#endif

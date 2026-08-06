#ifndef __TEXTURE_H__
#define __TEXTURE_H__

/*
Wrapper for OpenGL texture object
*/
typedef struct _texture_t {
	GLuint tex;
} texture_t;

/*
Loads in a texture from the specified file path

*/
BOOL
texture_load(texture_t *tex,
	const char *path);

/*
Binds the specified OpenGL texture
*/
void
texture_bind(texture_t tex);

/*
Frees memory associated with tex
*/
void
texture_destroy(texture_t tex);

#endif
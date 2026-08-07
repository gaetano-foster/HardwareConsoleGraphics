#ifndef __TEXTURE_H__
#define __TEXTURE_H__

/*
Wrapper for OpenGL texture object
*/
typedef struct _texture_t {
	GLuint tex;
} texture_t;

/*
Returns a heap allocated texture_t loaded in from path
that must be freed with texture_cleanup
*/
texture_t *
texture_load(const char *path);

/*
Binds the specified OpenGL texture
*/
void
texture_bind(texture_t *tex);

/*
Frees memory associated with tex
*/
void
texture_cleanup(texture_t *tex);

#endif
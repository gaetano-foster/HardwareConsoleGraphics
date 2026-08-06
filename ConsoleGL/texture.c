#include <glad/glad.h>
#include <Windows.h>
#include "stb_image.h"
#include "texture.h"

BOOL
texture_load(texture_t *tex,
	const char *path)
{
	glGenTextures(1, &tex->tex);
	glBindTexture(GL_TEXTURE_2D, tex->tex);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load and generate the texture
	int width, height, nrChannels;
	BYTE *data;
	if (!(data = stbi_load(path, &width, &height, &nrChannels, 0))) {
		return FALSE;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	return TRUE;
}

void
texture_bind(texture_t tex)
{
	glBindTexture(GL_TEXTURE_2D, tex.tex);
}

void
texture_destroy(texture_t tex)
{
	glDeleteTextures(1, &tex.tex);
}
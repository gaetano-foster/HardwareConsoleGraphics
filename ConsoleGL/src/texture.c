#include <Windows.h>
#include "stb_image.h"
#include "texture.h"

texture_t *
texture_load(const char *path)
{
	texture_t *tex;
	if (!(tex = malloc(sizeof(texture_t)))) {
		fprintf(stderr, "Failed to allocate memory for texture.");
		return NULL;
	}

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
		free(tex);
		return NULL;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	return tex;
}

void
texture_bind(texture_t *tex)
{
	glBindTexture(GL_TEXTURE_2D, tex->tex);
}

void
texture_cleanup(texture_t *tex)
{
	glDeleteTextures(1, &tex->tex);
	free(tex);
}
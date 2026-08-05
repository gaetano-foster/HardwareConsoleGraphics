#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "render_target.h"

static struct {
	GLuint fbo;
	GLuint color;
	GLuint rbo;
	GLuint width, height;
} target;

void
render_target_init(int width, int height)
{
	target.width = width;
	target.height = height;
	// generate frame buffer
	glGenFramebuffers(1, &target.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, target.fbo);
	// generate color texture buffer
	glGenTextures(1, &target.color);
	glBindTexture(GL_TEXTURE_2D, target.color);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		width,
		height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		target.color,
		0);
	// generate & bind render buffer to frame buffer
	glGenRenderbuffers(1, &target.rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, target.rbo);
	glRenderbufferStorage(
		GL_RENDERBUFFER,
		GL_DEPTH24_STENCIL8,
		width,
		height);
	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER,
		GL_DEPTH_STENCIL_ATTACHMENT,
		GL_RENDERBUFFER,
		target.rbo);
	EXPECT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void
render_target_bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, target.fbo);
	glViewport(0, 0, target.width, target.height);
}

void
render_target_unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void
render_target_cleanup()
{
	glDeleteFramebuffers(1, &target.fbo);
	glDeleteTextures(1, &target.color);
	glDeleteRenderbuffers(1, &target.rbo);
}
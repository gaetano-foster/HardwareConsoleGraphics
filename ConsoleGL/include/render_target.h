#ifndef __RENDER_TARGET_H__
#define __RENDER_TARGET_H__

/*
Initializes the render target
*/
void
render_target_init();

/*
Binds the render target's framebuffer
*/
void
render_target_bind();

/*
Unbinds the render target's framebuffer
*/
void
render_target_unbind();

/*
Frees memory associated with render target
*/
void
render_target_cleanup();

#endif
#ifndef __CONTEXT_H__
#define __CONTEXT_H__

/*
Opens an invisible SDL Window and initializes OpenGL

Parameters:
	- int width: the desired width of the window in pixels
	- int height: the desired height of the window in pixels

Return Value:
	- FALSE (0): Indicates failure
	- TRUE	(1): Indicates success
*/
BOOL
context_init(int width, int height);

/*
Frees memory allocated by context_init and calls SDL_Quit
*/
void
context_destroy();

/*
Calls SDL_GL_SwapWindow on the invisible SDL Window
*/
void
context_swap();

#endif
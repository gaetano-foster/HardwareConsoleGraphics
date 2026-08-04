#include <SDL.h>
#include <stdio.h>
#include <glad/glad.h>
typedef int BOOL;
#define FALSE 0
#define TRUE 1
#include "context.h"

SDL_Window *win = NULL;
SDL_GLContext context = NULL;

BOOL
context_init(int width, int height) 
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Failed to init video: %s\n", SDL_GetError());
		SDL_Quit();
		return FALSE;
	}

	// window width and height are scaled up because S_WIDTH and S_HEIGHT 
	// are the screen width/height in characters, not pixels
	if (!(win = SDL_CreateWindow("", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		width, height, 
		SDL_WINDOW_OPENGL 
#ifdef _DEBUG
		| SDL_WINDOW_SHOWN))) {
#else
		| SDL_WINDOW_HIDDEN))) {
#endif
		fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
		SDL_Quit();
		return 0;
	}

	if (!(context = SDL_GL_CreateContext(win))) {
		fprintf(stderr, "Failed to create context: %s\n", SDL_GetError());
		SDL_DestroyWindow(win);
		SDL_Quit();
		return FALSE;
	}

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		fprintf(stderr, "Failed to initialize GLAD");
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return FALSE;
	}
	glEnable(GL_DEPTH_TEST); 
	SDL_GL_SetSwapInterval(0);

	return TRUE;
}

void
context_destroy()
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(win);
	SDL_Quit();
}

void
context_swap()
{
	SDL_GL_SwapWindow(win);
}
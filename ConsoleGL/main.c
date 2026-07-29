#include <stdio.h>
#include <glad/glad.h>
#include <SDL.h>
#include <Windows.h>
#include "expect.h"
#include "mesh.h"
#include "conscr.h"

BOOL
init_window_and_context(SDL_Window **win,
						SDL_GLContext *context) 
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

	// window width and height are scaled up because S_WIDTH and S_HEIGHT are the screen width/height in characters, not pixels
	if (!(*win = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, S_WIDTH, S_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN))) {
		fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
		SDL_Quit();
		return 0;
	}

	if (!(*context = SDL_GL_CreateContext(*win))) {
		fprintf(stderr, "Failed to create context: %s\n", SDL_GetError());
		SDL_DestroyWindow(*win);
		SDL_Quit();
		return FALSE;
	}

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		fprintf(stderr, "Failed to initialize GLAD");
		SDL_GL_DeleteContext(*context);
		SDL_DestroyWindow(*win);
		SDL_Quit();
		return FALSE;
	}

	return TRUE;
}

void
destroy_window_and_context(SDL_Window **win,
						   SDL_GLContext *context)
{
	SDL_GL_DeleteContext(*context);
	SDL_DestroyWindow(*win);
	SDL_Quit();
}

int
main(int argc,
	 char **argv)
{
	// sdl window constructs
	SDL_Window *win;
	SDL_GLContext context;
	SDL_Event event;
	// main loop
	BOOL running = TRUE;
	// opengl data
	GLuint shader_program;
	// triangle data
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f 
	};
	// create mesh from vertices
	struct mesh_t triangle = {
		.vertices = vertices,
		.size = sizeof(vertices)
	};
	struct conscr_t screen;
	
	// init window and context
	EXPECT(init_window_and_context(&win, &context));
	EXPECT(compile_shaders(&shader_program));
	init_mesh_buffers(&triangle);

	// init console screen buffer
	EXPECT(conscr_init(&screen));

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = FALSE;
			}
		}
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader_program);
		render_mesh(triangle);
		conscr_render(&screen);
		SDL_GL_SwapWindow(win);
	}

	destroy_window_and_context(&win, &context);
	conscr_destroy(&screen);
	return 0;
}
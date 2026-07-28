#include <stdio.h>
#include <glad/glad.h>
#include <SDL.h>
#include <Windows.h>
#include "mesh.h"

#define S_WIDTH		(120)
#define S_HEIGHT	(40)
#define EXPECT(B)								\
    do {										\
        if (!(B)) {								\
			fprintf(stderr,						\
                "%s:%d: EXPECT(%s) failed\n",	\
                __FILE__, __LINE__, #B);		\
            exit(EXIT_FAILURE);					\
		}										\
    } while (0)

// defining one pixel as b, g, r and no alpha
struct bgr_t {
	BYTE b;
	BYTE g;
	BYTE r;
};

struct console_screen_t {
	// buffers
	CHAR_INFO *front_buffer;
	// handles
	HANDLE console_handle;
	HANDLE original;
};

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
	if (!(*win = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, S_WIDTH * 8, S_HEIGHT * 16, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN))) {
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

BOOL
init_console_screen(struct console_screen_t *screen)
{
	screen->original = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!(screen->front_buffer = malloc(sizeof(CHAR_INFO) * S_WIDTH * S_HEIGHT))) {
		fprintf(stderr, "Failed to allocate memory for screen buffer.\n");
		return FALSE;
	}
	screen->console_handle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (screen->console_handle == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Failed to create console screen handle.\n");
		free(screen->front_buffer);
		return FALSE;
	}
	SetConsoleActiveScreenBuffer(screen->console_handle);
	return TRUE;
}

void
console_write_buffer(struct console_screen_t *screen)
{
	SMALL_RECT write_region = { 0, 0, S_WIDTH - 1, S_HEIGHT - 1 };
	WriteConsoleOutputW(
		screen->console_handle, 
		screen->front_buffer, 
		(COORD) { S_WIDTH, S_HEIGHT }, 
		(COORD) { 0, 0 }, 
		&write_region
	);
}

void
destroy_console_screen(struct console_screen_t *screen)
{
	SetConsoleActiveScreenBuffer(screen->original);
	CloseHandle(screen->console_handle);
	free(screen->front_buffer);
}

CHAR_INFO 
bgr_to_ascii(struct bgr_t bgr) 
{
	const int MAX_VALUE = 765; // 255 + 255 + 255
	// darkest to lightest characters
	// TODO: adjust characters for luminance for more accurate lighting
	const WCHAR *palette = L" `.-':_,^=;>+!rc/z?sLTv)J7(|F{CfI31tlu[neoZ5Yxjya2ESwkP6h9d4VpOGbUAXHm8RD#$Bg0MNWQ%&@@";
	int value = bgr.b + bgr.g + bgr.r;
	double percent = (double)value / (double)MAX_VALUE;
	
	// normalize color values from pixel
	BYTE b = bgr.b, g = bgr.g, r = bgr.r;
	b = b < 127 ? 0 : FOREGROUND_BLUE;
	g = g < 127 ? 0 : FOREGROUND_GREEN;
	r = r < 127 ? 0 : FOREGROUND_RED;

	return (CHAR_INFO) {
		.Char.UnicodeChar = palette[(int)(percent * 85)],
		.Attributes = r | g | b
	};
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
	DWORD shader_program;
	// triangle data
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};
	// create mesh from vertices
	struct mesh_t triangle = {
		.vertices = vertices,
		.size = sizeof(vertices)
	};
	// windows console window constructs
	struct console_screen_t screen;
	
	// init window and context
	EXPECT(init_window_and_context(&win, &context));
	EXPECT(compile_shaders(&shader_program));
	init_mesh_buffers(&triangle);

	// init console screen buffer
	EXPECT(init_console_screen(&screen));

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
		SDL_GL_SwapWindow(win);
	}

	destroy_window_and_context(&win, &context);
	destroy_console_screen(&screen);
	return 0;
}
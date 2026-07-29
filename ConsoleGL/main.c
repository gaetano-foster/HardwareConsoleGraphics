#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <glad/glad.h>
#include <SDL.h>
#include <Windows.h>
#include <assert.h>
#include "mesh.h"

#define S_WIDTH			(120)
#define S_HEIGHT		(40)
#define CELL_MAX_BYTES	(32)

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
typedef struct bgr_t {
	BYTE b;
	BYTE g;
	BYTE r;
};

struct console_screen_t {
	// buffers
	BYTE *pixel_buffer;
	char *frame_buffer;
	// handles
	HANDLE console_handle;
	HANDLE original;
	// junk data
	DWORD bytes_written;
};

// saving because it might be useful later, but now unused
CHAR_INFO 
bgr_to_ascii(struct bgr_t bgr) 
{
	const int MAX_VALUE = 765; // 255 + 255 + 255
	// darkest to lightest characters
	static const WCHAR *palette = L" ▏▎▍▌▋▊▉█";
	const size_t palette_length = wcslen(palette);
	double value = bgr.b + bgr.g + bgr.r;
	int index = (value / MAX_VALUE) * palette_length;

	// threshold colors into attribute colors
	WORD attr = 0;

	if (bgr.b > 100)
		attr |= FOREGROUND_BLUE;

	if (bgr.g > 150)
		attr |= FOREGROUND_GREEN;

	if (bgr.r > 120)
		attr |= FOREGROUND_RED;

	if ((bgr.b + bgr.g + bgr.r) / 3 > 127) {
		attr |= FOREGROUND_INTENSITY;
	}

	return (CHAR_INFO) {
		.Char.UnicodeChar = palette[index],
		.Attributes = attr
	};
}

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

BOOL
init_console_screen(struct console_screen_t *screen)
{
	// store original screen
	screen->original = GetStdHandle(STD_OUTPUT_HANDLE);

	// allocate pixel buffer
	if (!(screen->pixel_buffer = malloc(3 * S_WIDTH * S_HEIGHT))) {
		fprintf(stderr, "Failed to allocate memory for pixel buffer.\n");
		return FALSE;
	}

	// allocate frame buffer
	if (!(screen->frame_buffer = malloc(CELL_MAX_BYTES * S_WIDTH * S_HEIGHT))) {
		fprintf(stderr, "Failed to allocate memory for frame buffer.\n");
		free(screen->pixel_buffer);
		return FALSE;
	}

	// create and focus console screen buffer
	screen->console_handle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (screen->console_handle == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Failed to create console screen handle.\n");
		free(screen->pixel_buffer);
		return FALSE;
	}

	DWORD mode = 0;
	if (GetConsoleMode(screen->console_handle, &mode)) {
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleMode(screen->console_handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	}

	SetConsoleActiveScreenBuffer(screen->console_handle);
	return TRUE;
}

void
console_write_buffer(struct console_screen_t *screen)
{
	// write frame buffer data to screen buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_FRONT);

	// read raw color data into pixel buffer
	glReadPixels(
		0, 0, 
		S_WIDTH, S_HEIGHT, 
		GL_BGR, 
		GL_UNSIGNED_BYTE, 
		screen->pixel_buffer);

	struct bgr_t *buf = (struct bgr_t *)(screen->pixel_buffer);
	char *fb = screen->frame_buffer;
	// move cursor to 0, 0
	fb += sprintf(fb, "\x1b[H");

	// encode color + character data into framebuffer =
	for (int y = 0; y < S_HEIGHT; y++) {
		for (int x = 0; x < S_WIDTH; x++) {
			struct bgr_t color = buf[(S_HEIGHT - 1 - y) * S_WIDTH + x];
			fb += sprintf(
				fb,
				"\x1b[38;2;%d;%d;%dm█",
				color.r, color.g, color.b);
		}
	}

	DWORD chars_to_write = fb - screen->frame_buffer; // difference between fb pointer and starting frame buffer pointer = number of characters to write
	WriteConsoleA(
		screen->console_handle,
		screen->frame_buffer,
		chars_to_write,
		&screen->bytes_written,
		NULL);

	glPixelStorei(GL_PACK_ALIGNMENT, 4);
}

void
destroy_console_screen(struct console_screen_t *screen)
{
	SetConsoleActiveScreenBuffer(screen->original);
	CloseHandle(screen->console_handle);
	free(screen->pixel_buffer);
	free(screen->frame_buffer);
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
		console_write_buffer(&screen);
		SDL_GL_SwapWindow(win);
	}

	destroy_window_and_context(&win, &context);
	destroy_console_screen(&screen);
	return 0;
}
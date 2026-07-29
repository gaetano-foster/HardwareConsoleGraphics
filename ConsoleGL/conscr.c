#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <glad/glad.h>
#include <Windows.h>
#include "conscr.h"

static void
resize_console(HANDLE console_handle)
{
	// change font size
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(console_handle, FALSE, &cfi);
	cfi.dwFontSize.X = 0; // system auto calculates width
	cfi.dwFontSize.Y = FONT_SIZE; // new font height in pixels
	wcscpy_s(cfi.FaceName, LF_FACESIZE, L"Consolas");
	SetCurrentConsoleFontEx(console_handle, FALSE, &cfi);

	// set screen buffer size & physical window rect size
	SetConsoleScreenBufferSize(console_handle, (COORD) { S_WIDTH, S_HEIGHT });
	SetConsoleWindowInfo(console_handle, TRUE, &(SMALL_RECT) { 0, 0, S_WIDTH - 1, S_HEIGHT - 1 });
}

BOOL
conscr_init(struct conscr_t *screen)
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
	resize_console(screen->console_handle);
	return TRUE;
}

void
conscr_render(struct conscr_t *screen)
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

	// encode color + character data into framebuffer
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
conscr_destroy(struct conscr_t *screen)
{
	SetConsoleActiveScreenBuffer(screen->original);
	CloseHandle(screen->console_handle);
	free(screen->pixel_buffer);
	free(screen->frame_buffer);
}

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
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <glad/glad.h>
#include <Windows.h>
#include "conscr.h"

static struct _conscr_t {
	// char info buffers
	CHAR_INFO *ci_buffer;
	// true color buffers
	BYTE *pixel_buffer;
	char *frame_buffer;
	// handles
	HANDLE console_handle;
	HANDLE original;
	// junk data
	DWORD bytes_written;
} conscr_screen;

static void
resize_console(HANDLE console_handle)
{
	// TODO: Make font, font size, and window dimensions configurable
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
conscr_init()
{
	// store original screen
	conscr_screen.original = GetStdHandle(STD_OUTPUT_HANDLE);

	// allocate pixel buffer
	if (!(conscr_screen.pixel_buffer = malloc(3 * S_WIDTH * S_HEIGHT))) {
		fprintf(stderr, "Failed to allocate memory for pixel buffer.\n");
		return FALSE;
	}

	// allocate frame buffer
	if (!(conscr_screen.frame_buffer = malloc(CELL_MAX_BYTES * S_WIDTH * S_HEIGHT))) {
		fprintf(stderr, "Failed to allocate memory for frame buffer.\n");
		free(conscr_screen.pixel_buffer);
		return FALSE;
	}

	// allocate char_info mode buffers
	if (!(conscr_screen.ci_buffer = malloc(sizeof(CHAR_INFO) * S_WIDTH * S_HEIGHT))) {
		fprintf(stderr, "Failed to allocate memory for front buffer.\n");
		free(conscr_screen.pixel_buffer);
		free(conscr_screen.frame_buffer);
		return FALSE;
	}

	// create and focus console screen buffer
	conscr_screen.console_handle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (conscr_screen.console_handle == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Failed to create console screen handle.\n");
		free(conscr_screen.pixel_buffer);
		free(conscr_screen.frame_buffer);
		free(conscr_screen.ci_buffer);
		return FALSE;
	}

	DWORD mode = 0;
	if (GetConsoleMode(conscr_screen.console_handle, &mode)) {
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleMode(conscr_screen.console_handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	}

	SetConsoleActiveScreenBuffer(conscr_screen.console_handle);
	resize_console(conscr_screen.console_handle);
	return TRUE;
}

void
conscr_renderci()
{
	// write frame buffer data to screen buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, S_WIDTH, S_HEIGHT, GL_BGR, GL_UNSIGNED_BYTE, conscr_screen.pixel_buffer);
	struct bgr_t *buf = (struct bgr_t *)(conscr_screen.pixel_buffer);
	// TODO: find better way of doing this
	for (int y = 0; y < S_HEIGHT; y++) {
		for (int x = 0; x < S_WIDTH; x++) {
			conscr_screen.ci_buffer[y * S_WIDTH + x] = bgr_to_ascii(buf[((S_HEIGHT - 1 - y) * S_WIDTH + x)]);
		}
	}
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	// write screen buffer data to console
	SMALL_RECT write_region = { 0, 0, S_WIDTH - 1, S_HEIGHT - 1 };
	WriteConsoleOutputW(
		conscr_screen.console_handle, 
		conscr_screen.ci_buffer, 
		(COORD) { S_WIDTH, S_HEIGHT }, 
		(COORD) { 0, 0 }, 
		&write_region
	);
}

void
conscr_render()
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
		conscr_screen.pixel_buffer);

	struct bgr_t *buf = (struct bgr_t *)(conscr_screen.pixel_buffer);
	char *fb = conscr_screen.frame_buffer;
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

	DWORD chars_to_write = fb - conscr_screen.frame_buffer; // difference between fb pointer and starting frame buffer pointer = number of characters to write
	WriteConsoleA(
		conscr_screen.console_handle,
		conscr_screen.frame_buffer,
		chars_to_write,
		&conscr_screen.bytes_written,
		NULL);

	glPixelStorei(GL_PACK_ALIGNMENT, 4);
}

void
conscr_destroy()
{
	SetConsoleActiveScreenBuffer(conscr_screen.original);
	CloseHandle(conscr_screen.console_handle);
	free(conscr_screen.pixel_buffer);
	free(conscr_screen.frame_buffer);
	free(conscr_screen.ci_buffer);
}

// TODO: Use lookup table and match colors to nearest instead for better fidelity
CHAR_INFO 
bgr_to_ascii(struct bgr_t bgr) 
{
	const int MAX_BRIGHTNESS = 255;
	// darkest to lightest characters
	static const WCHAR *palette = L".,:-=+*#%@";
	const size_t palette_length = wcslen(palette);
	double brightness = 
		0.0722 * bgr.b + 
		0.7152 * bgr.g + 
		0.2126 * bgr.r;

	int index = ((brightness / MAX_BRIGHTNESS) * (palette_length));

	// threshold colors into attribute colors
	WORD attr = 0;

	if (bgr.b > 140) attr |= FOREGROUND_BLUE;
	if (bgr.g > 128) attr |= FOREGROUND_GREEN;
	if (bgr.r > 110) attr |= FOREGROUND_RED;
	if ((brightness) > 216) attr |= FOREGROUND_INTENSITY;

	return (CHAR_INFO) {
		.Char.UnicodeChar = palette[index],
		.Attributes = attr
	};
}
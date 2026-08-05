#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <glad/glad.h>
#include <Windows.h>
#include "conscr.h"
#include "utils.h"

static struct _conscr_t {
	// HUD
	char hud_message[HUD_MAX_LEN];
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
} conscr;

static struct {
	double time_reading, time_writing, time_printing, time_gl;
	int frames;
} debug_accumulator;

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
	conscr.original = GetStdHandle(STD_OUTPUT_HANDLE);

	// allocate pixel buffer
	if (!(conscr.pixel_buffer = malloc(3 * S_WIDTH * S_HEIGHT))) {
		fprintf(stderr, "Failed to allocate memory for pixel buffer.\n");
		return FALSE;
	}

	// allocate frame buffer
	if (!(conscr.frame_buffer = malloc(CELL_MAX_BYTES * S_WIDTH * S_HEIGHT))) {
		fprintf(stderr, "Failed to allocate memory for frame buffer.\n");
		free(conscr.pixel_buffer);
		return FALSE;
	}

	// allocate char_info mode buffers
	if (!(conscr.ci_buffer = malloc(sizeof(CHAR_INFO) * S_WIDTH * S_HEIGHT))) {
		fprintf(stderr, "Failed to allocate memory for front buffer.\n");
		free(conscr.pixel_buffer);
		free(conscr.frame_buffer);
		return FALSE;
	}

	// create and focus console screen buffer
	conscr.console_handle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (conscr.console_handle == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Failed to create console screen handle.\n");
		free(conscr.pixel_buffer);
		free(conscr.frame_buffer);
		free(conscr.ci_buffer);
		return FALSE;
	}

	DWORD mode = 0;
	if (GetConsoleMode(conscr.console_handle, &mode)) {
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleMode(conscr.console_handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	}

	SetConsoleActiveScreenBuffer(conscr.console_handle);
	resize_console(conscr.console_handle);
	return TRUE;
}

void
conscr_renderhud() 
{
	char message[HUD_MAX_LEN + 20] = "\x1b[38;2;255;255;255m";
	strcat(message, conscr.hud_message);

	SetConsoleCursorPosition(
		conscr.console_handle,
		(COORD){ 0, 0 }
	);
	WriteConsoleA(
		conscr.console_handle, 
		message, 
		strlen(message),
		&conscr.bytes_written, 
		NULL
	);
}

void
conscr_rendercihud() 
{
	SetConsoleCursorPosition(
		conscr.console_handle,
		(COORD){ 0, 0 }
	);
	WriteConsoleA(
		conscr.console_handle, 
		conscr.hud_message, 
		HUD_MAX_LEN, 
		&conscr.bytes_written, 
		NULL
	);
}

void
conscr_renderci()
{
	double write_time_millis = 0, print_time_millis = 0;

	// write frame buffer data to screen buffer
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(
		0, 0, 
		S_WIDTH, S_HEIGHT, 
		GL_BGR, 
		GL_UNSIGNED_BYTE, 
		conscr.pixel_buffer);

	struct bgr_t *buf = (struct bgr_t *)(conscr.pixel_buffer);
	// TODO: find better way of doing this
	TIME(write_time_millis,
	for (int y = 0; y < S_HEIGHT; y++) {
		for (int x = 0; x < S_WIDTH; x++) {
			conscr.ci_buffer[y * S_WIDTH + x] = bgr_to_ascii(buf[(y * S_WIDTH + x)]);
		}
	}
	);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	CONSCR_HUD_CAT("WRITE TIME: %.6f  \n", write_time_millis);

	// write screen buffer data to console
	TIME(print_time_millis, 
	SMALL_RECT write_region = {0, 0, S_WIDTH - 1, S_HEIGHT - 1};
	WriteConsoleOutput(
		conscr.console_handle, 
		conscr.ci_buffer, 
		(COORD) { S_WIDTH, S_HEIGHT }, 
		(COORD) { 0, 0 }, 
		&write_region);
	);
	CONSCR_HUD_CAT("PRINT TIME: %.6f  \n", print_time_millis);

	debug_accumulator.time_writing += write_time_millis;
	debug_accumulator.time_printing += print_time_millis;
	debug_accumulator.frames++;
}

void
conscr_render()
{
	double write_time_millis = 0, print_time_millis = 0;

	// write frame buffer data to screen buffer
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(
		0, 0, 
		S_WIDTH, S_HEIGHT, 
		GL_BGR, 
		GL_UNSIGNED_BYTE, 
		conscr.pixel_buffer);

	struct bgr_t *buf = (struct bgr_t *)(conscr.pixel_buffer);
	char *fb = conscr.frame_buffer;
	// move cursor to 0, 0
	TIME(write_time_millis, 
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
	);
	CONSCR_HUD_CAT("WRITE TIME: %.6f  \n", write_time_millis);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);

	TIME(print_time_millis,
	DWORD chars_to_write = fb - conscr.frame_buffer; // difference between fb pointer and starting frame buffer pointer = number of characters to write
	WriteConsoleA(
		conscr.console_handle,
		conscr.frame_buffer,
		chars_to_write,
		&conscr.bytes_written,
		NULL);
	);
	CONSCR_HUD_CAT("PRINT TIME: %.6f  \n", print_time_millis);

	debug_accumulator.time_writing += write_time_millis;
	debug_accumulator.time_printing += print_time_millis;
	debug_accumulator.frames++;
}

void
conscr_destroy()
{
	SetConsoleActiveScreenBuffer(conscr.original);
	CloseHandle(conscr.console_handle);
	free(conscr.pixel_buffer);
	free(conscr.frame_buffer);
	free(conscr.ci_buffer);
	printf("AVG WRITING TIME: %.6f\nAVG PRINTING TIME: %.6f\n", debug_accumulator.time_writing / debug_accumulator.frames, debug_accumulator.time_printing / debug_accumulator.frames);
}

void
conscr_sethud(const char *msg)
{
	strcpy(conscr.hud_message, msg);
}

const char *
conscr_hud()
{
	return conscr.hud_message;
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
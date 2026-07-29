#ifndef __CONSOLE_SCREEN__
#define __CONSOLE_SCREEN__

#define S_WIDTH			(120)
#define S_HEIGHT		(40)
#define CELL_MAX_BYTES	(32)

// defining one pixel as b, g, r and no alpha
typedef struct bgr_t {
	BYTE b;
	BYTE g;
	BYTE r;
};

struct conscr_t {
	// buffers
	BYTE *pixel_buffer;
	char *frame_buffer;
	// handles
	HANDLE console_handle;
	HANDLE original;
	// junk data
	DWORD bytes_written;
};

BOOL
conscr_init(struct conscr_t *screen);

void
conscr_render(struct conscr_t *screen);

void
conscr_destroy(struct conscr_t *screen);

// saving because it might be useful later, but now unused
CHAR_INFO 
bgr_to_ascii(struct bgr_t bgr);

#endif
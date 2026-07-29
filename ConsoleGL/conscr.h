#ifndef __CONSOLE_SCREEN__
#define __CONSOLE_SCREEN__

#define S_WIDTH			(120)
#define S_HEIGHT		(40)
#define FONT_SIZE		(16)		
#define CELL_MAX_BYTES	(32)

// defining one pixel as b, g, r and no alpha
typedef struct bgr_t {
	BYTE b;
	BYTE g;
	BYTE r;
};

BOOL
conscr_init();

void
conscr_render();

void
conscr_destroy();

// saving because it might be useful later, but now unused
CHAR_INFO 
bgr_to_ascii(struct bgr_t bgr);

#endif
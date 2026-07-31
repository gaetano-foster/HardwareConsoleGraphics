#ifndef __CONSOLE_SCREEN__
#define __CONSOLE_SCREEN__

#define S_WIDTH			(120)
#define S_HEIGHT		(40)
#define FONT_SIZE		(16)		
#define CELL_MAX_BYTES	(32)

/*
Represents one 24-bit-pixel in BGR format. 3 BYTEs
*/
struct bgr_t {
	BYTE b;
	BYTE g;
	BYTE r;
};

/*
Initializes Console Screen and allocates memory

Return Values:
	- FALSE (0): Indicates failure
	- TRUE	(1): Indicates success
*/
BOOL
conscr_init();

/*
Encodes OpenGL frame buffer 0 to Console Screen frame buffer 
and writes Console Screen frame buffer to the console using
true color escape codes
*/
void
conscr_render();

/*
Encodes OpenGL frame buffer 0 to Console Screen frame buffer 
and writes Console Screen frame buffer to the console using
CHAR_INFO
*/
void
conscr_renderci();

/* 
Frees memory used by the Console Screen and returns
the console's screen buffer to its original state
*/
void
conscr_destroy();

// CURRENTLY UNUSED
CHAR_INFO 
bgr_to_ascii(struct bgr_t bgr);

#endif
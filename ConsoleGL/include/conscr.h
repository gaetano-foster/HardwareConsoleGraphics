#ifndef __CONSOLE_SCREEN__
#define __CONSOLE_SCREEN__
	
#define CELL_MAX_BYTES		(32)
#define HUD_MAX_LEN			(256)

#define CONSCR_HUD_FMT(FMT, ...) \
do { \
	char message[HUD_MAX_LEN]; \
	sprintf(message, FMT, __VA_ARGS__); \
	conscr_sethud(message); \
} while(0)

#define CONSCR_HUD_CAT(FMT, ...) \
do { \
	char message[HUD_MAX_LEN]; \
	sprintf(message, FMT, __VA_ARGS__); \
	strcat(message, conscr_hud());	\
	conscr_sethud(message); \
} while(0)

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
Wrapper around context_init()
*/
void
conscr_swap();

/*
Reads pixels from render_target framebuffer and
writes them to the console screen
*/
void
conscr_render();

/*
Defines the HUD message in the Console Screen
*/
void
conscr_sethud(const char *msg);

/*
Returns a pointer to the HUD message in the Console Screen
*/
const char *
conscr_hud();

/*
Writes the hud message to the console
*/
void
conscr_renderhud();

/* 
Frees memory used by the Console Screen and returns
the console's screen buffer to its original state
*/
void
conscr_destroy();

/*
Convert BGR into closest CHAR_INFO

glyph used for brightness,
attributes used for color
*/
CHAR_INFO 
bgr_to_ascii(struct bgr_t bgr);

#endif
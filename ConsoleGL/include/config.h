#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <Windows.h>

// Renderer flag
#define CMD_RENDER_TC			(0)
#define CMD_RENDER_CI			(1)
// Macros for easy access
#define S_WIDTH					(CONFIG->cols)
#define S_HEIGHT				(CONFIG->rows)
#define FONT_W					(CONFIG->fw)
#define FONT_H					(CONFIG->fh)
#define HUD_ENABLED				(CONFIG->hud)
#define CONSCR_MODE				(CONFIG->renderer)
#define TC_COLOR_MASK			(CONFIG->mask)
#define CI_PALETTE				(CONFIG->palette)
#define CI_THR_B				(CONFIG->b)
#define CI_THR_G				(CONFIG->g)
#define CI_THR_R				(CONFIG->r)
#define CI_THR_I				(CONFIG->i)

/*
Global configurable constants for the program
*/
typedef struct _cmd_render_config {
	// Screen Size
	BYTE cols;
	BYTE rows;
	// Font Size
	BYTE fw;
	BYTE fh;
	// HUD?
	BYTE hud;
	// Renderer
	BYTE renderer;
	// TC Options
	BYTE mask;
	// CI Options
	BYTE b, g, r, i;
	char palette[128];
} cmd_render_config;

extern const cmd_render_config * const CONFIG;

BOOL
configure(const char *path);

#endif
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "utils.h"
#include "config.h"

#define LINE_MAX	256

static cmd_render_config mut;
const cmd_render_config * const CONFIG = &mut;

BOOL
configure(const char *path)
{
	FILE *cfg;
	if (!(cfg = fopen(path, "r"))) {
		fprintf(stderr, "Couldn't read config file at %s\n", path);
		return FALSE;
	}

	char buf[LINE_MAX];

	while (fgets(buf, LINE_MAX, cfg)) {
		char *tok;
		if (buf[0] == '#') continue; // comment
		tok = strtok(buf, " ");

		if (strcmp(tok, "columns:") == 0) mut.cols = atoi(strtok(NULL, " "));
		else if (strcmp(tok, "rows:") == 0) mut.rows = atoi(strtok(NULL, " "));
		else if (strcmp(tok, "width:") == 0) mut.fw = atoi(strtok(NULL, " "));
		else if (strcmp(tok, "height:") == 0) mut.fh = atoi(strtok(NULL, " "));
		else if (strcmp(tok, "hud:") == 0) mut.hud = atoi(strtok(NULL, " "));
		else if (strcmp(tok, "renderer:") == 0) mut.renderer = atoi(strtok(NULL, " "));
		else if (strcmp(tok, "mask:") == 0) mut.mask = (BYTE)strtol(strtok(NULL, " "), NULL, 16);
		else if (strcmp(tok, "palette:") == 0) strcpy(mut.palette, strtok(strtok(NULL, " "), "\""));
		else if (strcmp(tok, "blue:") == 0) mut.b = atoi(strtok(NULL, " "));
		else if (strcmp(tok, "green:") == 0) mut.g = atoi(strtok(NULL, " "));
		else if (strcmp(tok, "red:") == 0) mut.r = atoi(strtok(NULL, " "));
		else if (strcmp(tok, "intense:") == 0) mut.i = atoi(strtok(NULL, " "));
	}
	return TRUE;
}
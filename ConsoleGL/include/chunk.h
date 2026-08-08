#ifndef __CHUNK_H__
#define __CHUNK_H__
#include <Windows.h>
#include "texture.h"

typedef enum _TILE_ID {
	TILE_AIR = 0,
	TILE_GRASS = 1
} TILE_ID;

typedef struct _tile_t {
	texture_t *tex;
	BOOL solid;
} tile_t;

void
tiles_init();

void
tiles_destroy();

void
tile_render(tile_t tile, 
	int x, int y, int z);

typedef TILE_ID chunk_t[16][16][256];

void
chunk_init(chunk_t chunk);

TILE_ID
chunk_tileat(chunk_t chunk,
	int x, int y, int z);

void
chunk_render(chunk_t chunk);

#endif
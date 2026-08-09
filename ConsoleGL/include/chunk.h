#ifndef __CHUNK_H__
#define __CHUNK_H__
#include <Windows.h>
#include "mesh.h"
#include "texture.h"

////
/// Tile
//

typedef enum _TILE_ID {
	TILE_AIR = 0,
	TILE_GRASS = 1,
	TILE_DIRT = 2
} TILE_ID;

/*
Structure storing the UV coordinates and collision flags of a tile
*/
typedef struct _tile_t {
	vec2 uv_set[6][4]; // 6 sides, 4 corners
	BOOL solid;
} tile_t;

/*
Initializes all tiles
*/
void
tiles_init();

/*
Frees memory associated with tiles
*/
void
tiles_destroy();

////
/// Chunk
//

typedef struct _chunk_t {
	TILE_ID tiles[16][16][256];
	// mesh data
	mesh_t *chunk_mesh;
	vertex_t *vertices;
	GLuint *indices;
	GLsizei vsize;
	GLsizei isize;
	GLsizei vcapacity;
	GLsizei icapacity;
} chunk_t;

/*
Initializes and builds initial chunk mesh
*/
void
chunk_init(chunk_t *chunk);

/*
Rebuilds chunk mesh
*/
void
chunk_rebuild(chunk_t *chunk);

/*
Returns the TILE_ID of the given coordinates
*/
inline TILE_ID
chunk_tileat(chunk_t *chunk,
	int x, int y, int z);

/*
Renders the chunk mesh
*/
void
chunk_render(chunk_t *chunk);

/*
Frees memory associated with chunk
*/
void 
chunk_cleanup(chunk_t *chunk);

#endif
#include "chunk.h"
#include "mesh.h"
#include "shader.h"
#include "object.h"
#define TILE_SET_LEN		256

static tile_t TILE_SET[TILE_SET_LEN];
static mesh_t *tile_mesh;
static shader_t *tile_shader;

// include shaders at compile time
#include "block.vs.h" // block_vs
#include "block.fs.h" // block_fs

void
tiles_init()
{
	// initialize cube mesh and tile shader
	tile_mesh = mesh_load("res/cube.obj");
	tile_shader = shader_compile(block_vs, block_fs);
	// initialize TILE_SET
	TILE_SET[TILE_AIR] = (tile_t) {
		.solid = FALSE,
		.tex = NULL
	};
	TILE_SET[TILE_GRASS] = (tile_t) {
		.solid = TRUE,
		.tex = texture_load("res/grass.jpg")
	};
}

void
tiles_destroy()
{
	mesh_cleanup(tile_mesh);
	shader_cleanup(tile_shader);
	for (int i = 0; i < TILE_SET_LEN; i++) {
		if (TILE_SET[i].tex) texture_cleanup(TILE_SET[i].tex);
	}
}

void
tile_render(tile_t tile,
	int x, int y, int z)
{
	if (tile.tex == NULL) return;
	object_t to_render = (object_t) {
		.mesh = tile_mesh,
		.shader = tile_shader,
		.texture = tile.tex
	};
	object_init(&to_render);
	object_setpos(&to_render, (vec3) { x, y, z });
	object_draw(&to_render);
}

void
chunk_init(chunk_t chunk)
{
	for (int y = 0; y < 256; y++) {
		for (int z = 0; z < 16; z++) {
			for (int x = 0; x < 16; x++) {
				chunk[x][z][y] = TILE_GRASS;
			}
		}
	}
}

TILE_ID
chunk_tileat(chunk_t chunk,
	int x,
	int y,
	int z)
{
	return chunk[x][z][y];
}

void
chunk_render(chunk_t chunk)
{
	for (int y = 0; y < 256; y++) {
		for (int z = 0; z < 16; z++) {
			for (int x = 0; x < 16; x++) {
				tile_render(TILE_SET[chunk_tileat(chunk, x, y, z)], x, y, z);
			}
		}
	}
}
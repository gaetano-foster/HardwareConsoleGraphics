#ifndef __WORLD_H__
#define __WORLD_H__
#include "chunk.h"
#define RENDER_DISTANCE			(2)

struct cnode_t {
	int x;
	int z;
	chunk_t *chunk;
	struct cnode_t *next;
};

typedef struct _world_t {
	size_t csize;
	size_t ccapacity;
	struct cnode_t **chunks;
} world_t;

world_t *
world_init();

void
world_cleanup(world_t *world);

chunk_t *
world_chunk(world_t *world,
	int x_offset, int z_offset);

chunk_t *
world_genchunk(world_t *world,
	int x_offset, int z_offset);

void 
world_tick(world_t *world);

void
world_render(world_t *world);

#endif
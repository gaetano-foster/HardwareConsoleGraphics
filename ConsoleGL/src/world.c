#include <Windows.h>
#include "utils.h"
#include "camera.h"
#include "world.h"

////
/// Hash map functions
//

static DWORD
hash(int x, int y) 
{
	DWORD h = 17;
	h = h * 31 + (DWORD)x;
	h = h * 31 + (DWORD)y;
	return h;
}

static void
world_rehash(world_t *world)
{
	int old_capacity = world->ccapacity;
	world->ccapacity *= 2;
	struct cnode_t **new_chunks =
		calloc(world->ccapacity, sizeof(struct cnode_t *));
	EXPECT(new_chunks);

	for (int i = 0; i < old_capacity; i++) {
		struct cnode_t *entry = world->chunks[i];
		while (entry) {
			struct cnode_t *next = entry->next;
			unsigned int slot =
				hash(entry->x, entry->z) % world->ccapacity;
			entry->next = new_chunks[slot];
			new_chunks[slot] = entry;
			entry = next;
		}
	}
	free(world->chunks);
	world->chunks = new_chunks;
}

////
/// World functions
//

world_t *
world_init()
{
	world_t *world = malloc(sizeof(world_t));
	EXPECT(world);
	world->ccapacity = 100;
	world->csize = 0;
	world->chunks = calloc(world->ccapacity, sizeof(struct cnode_t *));
	EXPECT(world->chunks);
	return world;
}

void
world_cleanup(world_t *world)
{
	for (int i = 0; i < world->ccapacity; i++) {
		struct cnode_t *entry = world->chunks[i];
		while (entry != NULL) {
			struct cnode_t* temp = entry;
			entry = entry->next;
			chunk_cleanup(temp->chunk);
			free(temp); 
		}
	}
	free(world);
}

chunk_t *
world_chunk(world_t *world,
	int x_offset, int z_offset)
{
	unsigned int slot = hash(x_offset, z_offset) % world->ccapacity;
	struct cnode_t* entry = world->chunks[slot];

	while (entry != NULL) {
		if (entry->x == x_offset && entry->z == z_offset) {
			return entry->chunk;
		}
		entry = entry->next;
	}
	return world_genchunk(world, x_offset, z_offset); 
}

chunk_t *
world_genchunk(world_t *world,
	int x_offset, int z_offset)
{
	if (world->csize >= world->ccapacity * 0.75) {
		world_rehash(world);
	}
	// allocate chunk
	chunk_t *chunk = chunk_init((vec2) { x_offset * CHUNK_X, z_offset * CHUNK_Z });
	EXPECT(chunk);
	// allocate node
	struct cnode_t *node = malloc(sizeof(struct cnode_t));
	EXPECT(node);
	// populate node
	node->x = x_offset;
	node->z = z_offset;
	node->chunk = chunk;

	unsigned int slot = hash(x_offset, z_offset) % world->ccapacity;
	node->next = world->chunks[slot];
	world->chunks[slot] = node;
	world->csize++;
	return chunk;
}

void 
world_tick(world_t *world)
{
	vec3 cam_pos;
	camera_pos(cam_pos);
	int cx = floor(cam_pos[0] / CHUNK_X);
	int cz = floor(cam_pos[2] / CHUNK_Z);

	// load/generate nearby chunks
	for (int z = cz - RENDER_DISTANCE; z <= cz + RENDER_DISTANCE; z++) {
		for (int x = cx - RENDER_DISTANCE; x <= cx + RENDER_DISTANCE; x++) {
			chunk_t *chunk = world_chunk(world, x, z);
			chunk->loaded = TRUE;
		}
	}

	// unload distant chunks
	for (int i = 0; i < world->ccapacity; i++) {
		struct cnode_t *entry = world->chunks[i];

		while (entry) {
			int dx = entry->x - cx;
			int dz = entry->z - cz;

			if (dx < -RENDER_DISTANCE || dx > RENDER_DISTANCE || dz < -RENDER_DISTANCE || dz > RENDER_DISTANCE)
				entry->chunk->loaded = FALSE;

			entry = entry->next;
		}
	}
}

void
world_render(world_t *world)
{
	for (int i = 0; i < world->ccapacity; i++) {
		struct cnode_t *entry = world->chunks[i];

		while (entry) {
			chunk_render(entry->chunk);
			entry = entry->next;
		}
	}
}
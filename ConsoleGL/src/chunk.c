#include "chunk.h"
#include "shader.h"
#include "object.h"
#include "utils.h"
#define TILE_SET_LEN				(256)
#define ATLAS_OFFSET(X, Y)			{ ((float)X / (float)16), ((float)(1.0f - Y) / (float)16) }
#define UV_COPY(v)					{ (v)[0], (v)[1] }

static tile_t TILE_SET[TILE_SET_LEN];
static shader_t *tile_shader;
static texture_t *atlas;
static mesh_t chunk_mesh;

// include shaders at compile time
#include "block.vs.h" // block_vs
#include "block.fs.h" // block_fs

////
/// Tile Implementation
//

void
tiles_init()
{
	// initialize cube mesh and tile shader
	atlas = texture_load("res/atlas.jpg");
	tile_shader = shader_compile(block_vs, block_fs);
	// initialize TILE_SET
	TILE_SET[TILE_AIR] = (tile_t) {
		.solid = FALSE,
		.uv_set = 0
	};
	TILE_SET[TILE_GRASS] = (tile_t) {
		.solid = TRUE,
		.uv_set = {
			{ ATLAS_OFFSET(0, 0), ATLAS_OFFSET(1, 0), ATLAS_OFFSET(0, 1), ATLAS_OFFSET(1, 1) }, // top (0)
			{ ATLAS_OFFSET(2, 0), ATLAS_OFFSET(3, 0), ATLAS_OFFSET(2, 1), ATLAS_OFFSET(3, 1) }, // bottom (1)
			{ ATLAS_OFFSET(1, 0), ATLAS_OFFSET(2, 0), ATLAS_OFFSET(1, 1), ATLAS_OFFSET(2, 1) }, // sides (2-5)
			{ ATLAS_OFFSET(1, 0), ATLAS_OFFSET(2, 0), ATLAS_OFFSET(1, 1), ATLAS_OFFSET(2, 1) },
			{ ATLAS_OFFSET(1, 0), ATLAS_OFFSET(2, 0), ATLAS_OFFSET(1, 1), ATLAS_OFFSET(2, 1) },
			{ ATLAS_OFFSET(1, 0), ATLAS_OFFSET(2, 0), ATLAS_OFFSET(1, 1), ATLAS_OFFSET(2, 1) }
		}
	};
}

void
tiles_destroy()
{
	texture_cleanup(atlas);
	shader_cleanup(tile_shader);
}

////
/// Chunk Implementation
//

void
chunk_init(chunk_t *chunk)
{
	// initialize memory
	chunk->chunk_mesh = malloc(sizeof(mesh_t));
	chunk->vertices = malloc(INT16_MAX * sizeof(vertex_t));
	chunk->indices = malloc(INT16_MAX * sizeof(GLuint));
	chunk->vcapacity = INT16_MAX;
	chunk->icapacity = INT16_MAX;
	EXPECT(chunk->chunk_mesh && chunk->vertices && chunk->indices);
	// initialize tiles
	for (int y = 0; y < 256; y++) {
		for (int z = 0; z < 16; z++) {
			for (int x = 0; x < 16; x++) {
				chunk->tiles[x][z][y] = TILE_GRASS;
			}
		}
	}
	// build chunk mesh
	chunk_rebuild(chunk);
}

void 
chunk_cleanup(chunk_t *chunk)
{
	mesh_cleanup(chunk->chunk_mesh);
	free(chunk->vertices);
	free(chunk->indices);
}

static void
add_top_face(chunk_t *chunk, 
	int x, int y, int z)
{
	GLsizei base = chunk->vsize;
	TILE_ID id = chunk_tileat(chunk, x, y, z);
	if (!chunk) return;

	if (chunk->vsize + 4 >= chunk->vcapacity) {
		chunk->vcapacity *= 2;
		chunk->vertices = realloc(chunk->vertices, chunk->vcapacity * sizeof(vertex_t));
		EXPECT(chunk->vertices);
	}

	if (chunk->isize + 6 >= chunk->icapacity) {
		chunk->icapacity *= 2;
		chunk->indices = realloc(chunk->indices, chunk->icapacity * sizeof(GLuint));
		EXPECT(chunk->indices);
	}

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x, y + 1, z },
		.uv = UV_COPY(TILE_SET[id].uv_set[0][0]),
		.normal = { 0.0, 1.0, 0.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x + 1, y + 1, z },
		.uv = UV_COPY(TILE_SET[id].uv_set[0][1]),
		.normal = { 0.0, 1.0, 0.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x, y + 1, z + 1 },
		.uv = UV_COPY(TILE_SET[id].uv_set[0][2]),
		.normal = { 0.0, 1.0, 0.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x + 1, y + 1, z + 1 },
		.uv = UV_COPY(TILE_SET[id].uv_set[0][3]),
		.normal = { 0.0, 1.0, 0.0 }
	};

	chunk->indices[chunk->isize++] = base + 3;
	chunk->indices[chunk->isize++] = base + 1;
	chunk->indices[chunk->isize++] = base + 0;
	chunk->indices[chunk->isize++] = base + 2;
	chunk->indices[chunk->isize++] = base + 3;
	chunk->indices[chunk->isize++] = base + 0;
}

static void 
add_bottom_face(chunk_t *chunk, 
	int x, int y, int z)
{
	GLsizei base = chunk->vsize;
	TILE_ID id = chunk_tileat(chunk, x, y, z);
	if (!chunk) return;

	if (chunk->vsize + 4 >= chunk->vcapacity) {
		chunk->vcapacity *= 2;
		chunk->vertices = realloc(chunk->vertices, chunk->vcapacity * sizeof(vertex_t));
		EXPECT(chunk->vertices);
	}

	if (chunk->isize + 6 >= chunk->icapacity) {
		chunk->icapacity *= 2;
		chunk->indices = realloc(chunk->indices, chunk->icapacity * sizeof(GLuint));
		EXPECT(chunk->indices);
	}

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x, y, z },
		.uv = UV_COPY(TILE_SET[id].uv_set[1][0]),
		.normal = { 0.0, -1.0, 0.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x + 1, y, z },
		.uv = UV_COPY(TILE_SET[id].uv_set[1][1]),
		.normal = { 0.0, -1.0, 0.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x, y, z + 1 },
		.uv = UV_COPY(TILE_SET[id].uv_set[1][2]),
		.normal = { 0.0, -1.0, 0.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x + 1, y, z + 1 },
		.uv = UV_COPY(TILE_SET[id].uv_set[1][3]),
		.normal = { 0.0, -1.0, 0.0 }
	};

	chunk->indices[chunk->isize++] = base + 3;
	chunk->indices[chunk->isize++] = base + 1;
	chunk->indices[chunk->isize++] = base + 0;
	chunk->indices[chunk->isize++] = base + 2;
	chunk->indices[chunk->isize++] = base + 3;
	chunk->indices[chunk->isize++] = base + 0;
}

static void
add_front_face(chunk_t *chunk, 
	int x, int y, int z)
{
	GLsizei base = chunk->vsize;
	TILE_ID id = chunk_tileat(chunk, x, y, z);
	if (!chunk) return;

	if (chunk->vsize + 4 >= chunk->vcapacity) {
		chunk->vcapacity *= 2;
		chunk->vertices = realloc(chunk->vertices, chunk->vcapacity * sizeof(vertex_t));
		EXPECT(chunk->vertices);
	}

	if (chunk->isize + 6 >= chunk->icapacity) {
		chunk->icapacity *= 2;
		chunk->indices = realloc(chunk->indices, chunk->icapacity * sizeof(GLuint));
		EXPECT(chunk->indices);
	}

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x, y, z + 1 },
		.uv = UV_COPY(TILE_SET[id].uv_set[2][0]),
		.normal = { 0.0, 0.0, 1.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x + 1, y, z + 1 },
		.uv = UV_COPY(TILE_SET[id].uv_set[2][1]),
		.normal = { 0.0, 0.0, 1.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x, y + 1, z + 1 },
		.uv = UV_COPY(TILE_SET[id].uv_set[2][2]),
		.normal = { 0.0, 0.0, 1.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x + 1, y + 1, z + 1 },
		.uv = UV_COPY(TILE_SET[id].uv_set[2][3]),
		.normal = { 0.0, 0.0, 1.0 }
	};

	chunk->indices[chunk->isize++] = base + 3;
	chunk->indices[chunk->isize++] = base + 1;
	chunk->indices[chunk->isize++] = base + 0;
	chunk->indices[chunk->isize++] = base + 2;
	chunk->indices[chunk->isize++] = base + 3;
	chunk->indices[chunk->isize++] = base + 0;
}

static void
add_back_face(chunk_t *chunk, 
	int x, int y, int z)
{
	GLsizei base = chunk->vsize;
	TILE_ID id = chunk_tileat(chunk, x, y, z);
	if (!chunk) return;

	if (chunk->vsize + 4 >= chunk->vcapacity) {
		chunk->vcapacity *= 2;
		chunk->vertices = realloc(chunk->vertices, chunk->vcapacity * sizeof(vertex_t));
		EXPECT(chunk->vertices);
	}

	if (chunk->isize + 6 >= chunk->icapacity) {
		chunk->icapacity *= 2;
		chunk->indices = realloc(chunk->indices, chunk->icapacity * sizeof(GLuint));
		EXPECT(chunk->indices);
	}

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x, y, z },
		.uv = UV_COPY(TILE_SET[id].uv_set[3][0]),
		.normal = { 0.0, 0.0, -1.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x + 1, y, z },
		.uv = UV_COPY(TILE_SET[id].uv_set[3][1]),
		.normal = { 0.0, 0.0, -1.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x, y + 1, z },
		.uv = UV_COPY(TILE_SET[id].uv_set[3][2]),
		.normal = { 0.0, 0.0, -1.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x + 1, y + 1, z },
		.uv = UV_COPY(TILE_SET[id].uv_set[3][3]),
		.normal = { 0.0, 0.0, -1.0 }
	};

	chunk->indices[chunk->isize++] = base + 3;
	chunk->indices[chunk->isize++] = base + 1;
	chunk->indices[chunk->isize++] = base + 0;
	chunk->indices[chunk->isize++] = base + 2;
	chunk->indices[chunk->isize++] = base + 3;
	chunk->indices[chunk->isize++] = base + 0;
}

static void
add_right_face(chunk_t *chunk, 
	int x, int y, int z)
{
	GLsizei base = chunk->vsize;
	TILE_ID id = chunk_tileat(chunk, x, y, z);
	if (!chunk) return;

	if (chunk->vsize + 4 >= chunk->vcapacity) {
		chunk->vcapacity *= 2;
		chunk->vertices = realloc(chunk->vertices, chunk->vcapacity * sizeof(vertex_t));
		EXPECT(chunk->vertices);
	}

	if (chunk->isize + 6 >= chunk->icapacity) {
		chunk->icapacity *= 2;
		chunk->indices = realloc(chunk->indices, chunk->icapacity * sizeof(GLuint));
		EXPECT(chunk->indices);
	}

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x + 1, y, z },
		.uv = UV_COPY(TILE_SET[id].uv_set[4][0]),
		.normal = { 1.0, 0.0, 0.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x + 1, y, z + 1 },
		.uv = UV_COPY(TILE_SET[id].uv_set[4][1]),
		.normal = { 1.0, 0.0, 0.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x + 1, y + 1, z },
		.uv = UV_COPY(TILE_SET[id].uv_set[4][2]),
		.normal = { 1.0, 0.0, 0.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x + 1, y + 1, z + 1 },
		.uv = UV_COPY(TILE_SET[id].uv_set[4][3]),
		.normal = { 1.0, 0.0, 0.0 }
	};

	chunk->indices[chunk->isize++] = base + 3;
	chunk->indices[chunk->isize++] = base + 1;
	chunk->indices[chunk->isize++] = base + 0;
	chunk->indices[chunk->isize++] = base + 2;
	chunk->indices[chunk->isize++] = base + 3;
	chunk->indices[chunk->isize++] = base + 0;
}

static void
add_left_face(chunk_t *chunk, 
	int x, int y, int z)
{
	GLsizei base = chunk->vsize;
	TILE_ID id = chunk_tileat(chunk, x, y, z);
	if (!chunk) return;

	if (chunk->vsize + 4 >= chunk->vcapacity) {
		chunk->vcapacity *= 2;
		chunk->vertices = realloc(chunk->vertices, chunk->vcapacity * sizeof(vertex_t));
		EXPECT(chunk->vertices);
	}

	if (chunk->isize + 6 >= chunk->icapacity) {
		chunk->icapacity *= 2;
		chunk->indices = realloc(chunk->indices, chunk->icapacity * sizeof(GLuint));
		EXPECT(chunk->indices);
	}

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x, y, z },
		.uv = UV_COPY(TILE_SET[id].uv_set[5][0]),
		.normal = { -1.0, 0.0, 0.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x, y, z + 1 },
		.uv = UV_COPY(TILE_SET[id].uv_set[5][1]),
		.normal = { -1.0, 0.0, 0.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x, y + 1, z },
		.uv = UV_COPY(TILE_SET[id].uv_set[5][2]),
		.normal = { -1.0, 0.0, 0.0 }
	};

	chunk->vertices[chunk->vsize++] = (vertex_t) {
		.position = { x, y + 1, z + 1 },
		.uv = UV_COPY(TILE_SET[id].uv_set[5][3]),
		.normal = { -1.0, 0.0, 0.0 }
	};

	chunk->indices[chunk->isize++] = base + 3;
	chunk->indices[chunk->isize++] = base + 1;
	chunk->indices[chunk->isize++] = base + 0;
	chunk->indices[chunk->isize++] = base + 2;
	chunk->indices[chunk->isize++] = base + 3;
	chunk->indices[chunk->isize++] = base + 0;
}

void
chunk_rebuild(chunk_t *chunk)
{
	chunk->vsize = 0;
	chunk->isize = 0;
	for (int y = 0; y < 256; y++) {
		for (int z = 0; z < 16; z++) {
			for (int x = 0; x < 16; x++) {
				if (!chunk_tileat(chunk, x, y + 1, z)) add_top_face(chunk, x, y, z);
				if (!chunk_tileat(chunk, x, y - 1, z)) add_bottom_face(chunk, x, y, z);
				if (!chunk_tileat(chunk, x, y, z + 1)) add_front_face(chunk, x, y, z);
				if (!chunk_tileat(chunk, x, y, z - 1)) add_back_face(chunk, x, y, z);
				if (!chunk_tileat(chunk, x + 1, y, z)) add_right_face(chunk, x, y, z);
				if (!chunk_tileat(chunk, x - 1, y, z)) add_left_face(chunk, x, y, z);
			}
		}
	}
	mesh_build(chunk->chunk_mesh, 
		chunk->vertices, 
		chunk->indices,
		chunk->vsize * sizeof(vertex_t),
		chunk->isize * sizeof(GLuint));
}

inline TILE_ID
chunk_tileat(chunk_t *chunk,
	int x,
	int y,
	int z)
{
	if (x > 15 || z > 15 || y > 255
		|| x < 0 || z < 0 || y < 0) return TILE_AIR;
	return chunk->tiles[x][z][y];
}

void
chunk_render(chunk_t *chunk)
{
	object_t chunk_object = (object_t){
		.mesh = chunk->chunk_mesh,
		.texture = atlas,
		.shader = tile_shader
	};
	object_init(&chunk_object);
	object_draw(&chunk_object);
}
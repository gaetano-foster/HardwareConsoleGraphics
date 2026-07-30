#include <stdio.h>
#include <glad/glad.h>
#include <SDL.h>
#include <Windows.h>
#include <cglm/cglm.h>
#include "expect.h"
#include "mesh.h"
#include "conscr.h"
#include "context.h"
#include "shader.h"
#include "object.h"
#include "camera.h"

#define SPEED		(0.05f)
#define SENS		(0.05f)

struct {
	BOOL W, A, S, D, SPACE, LSHIFT, UP, DOWN, LEFT, RIGHT;
} keys;

void
move_camera()
{
	if (keys.W)
		camera_translatel((vec3) { 0, 0, SPEED });
	if (keys.S)
		camera_translatel((vec3) { 0, 0, -SPEED });
	if (keys.A)
		camera_translatel((vec3) { -SPEED, 0, 0 });
	if (keys.D)
		camera_translatel((vec3) { SPEED, 0, 0 });
	if (keys.SPACE)
		camera_translatel((vec3) { 0, SPEED, 0 });
	if (keys.LSHIFT)
		camera_translatel((vec3) { 0, -SPEED, 0 });
	if (keys.UP)
		camera_rotate_rad((vec3) { SENS, 0, 0 });
	if (keys.DOWN)
		camera_rotate_rad((vec3) { -SENS, 0, 0 });
	if (keys.LEFT)
		camera_rotate_rad((vec3) { 0, -SENS, 0 });
	if (keys.RIGHT)
		camera_rotate_rad((vec3) { 0, SENS, 0 });
}

int
main(int argc,
	 char **argv)
{
	// main loop
	SDL_Event event;
	BOOL running = TRUE;

	EXPECT(context_init(S_WIDTH, S_HEIGHT));
	EXPECT(conscr_init());

	shader_t shader_program;
	EXPECT(shader_compile(&shader_program, "vert.glsl", "frag.glsl"));
	// triangle data
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f 
	};
	// create mesh from vertices
	mesh_t mesh = {
		.vertices = vertices,
		.size = sizeof(vertices)
	};
	mesh_init_buffers(&mesh);

	object_t triangle = {
		.mesh = &mesh,
		.shader = &shader_program
	};
	object_init(&triangle);
	object_setpos(&triangle, (vec3) { 0.0f, 0.0f, -1.0f });

	struct camera_config_t config = {
		.x = -0.5,
		.y = 0,
		.z = -0.5,
		.pitch = 0,
		.yaw = 0,
		.roll = 0,
		.fov = 90,
		.aspect = (float)S_WIDTH / (float)S_HEIGHT
	};
	camera_init(config);

	shader_use(&shader_program);

	while (running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT: 
				running = FALSE;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.scancode) {
				case SDL_SCANCODE_W:
					keys.W = TRUE;
					break;
				case SDL_SCANCODE_S:
					keys.S = TRUE;
					break;
				case SDL_SCANCODE_A:
					keys.A = TRUE;
					break;
				case SDL_SCANCODE_D:
					keys.D = TRUE;
					break;
				case SDL_SCANCODE_SPACE:
					keys.SPACE = TRUE;
					break;
				case SDL_SCANCODE_LSHIFT:
					keys.LSHIFT = TRUE;
					break;
				case SDL_SCANCODE_UP:
					keys.UP = TRUE;
					break;
				case SDL_SCANCODE_DOWN:
					keys.DOWN = TRUE;
					break;
				case SDL_SCANCODE_LEFT:
					keys.LEFT = TRUE;
					break;
				case SDL_SCANCODE_RIGHT:
					keys.RIGHT = TRUE;
					break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.scancode) {
				case SDL_SCANCODE_W:
					keys.W = FALSE;
					break;
				case SDL_SCANCODE_S:
					keys.S = FALSE;
					break;
				case SDL_SCANCODE_A:
					keys.A = FALSE;
					break;
				case SDL_SCANCODE_D:
					keys.D = FALSE;
					break;
				case SDL_SCANCODE_SPACE:
					keys.SPACE = FALSE;
					break;
				case SDL_SCANCODE_LSHIFT:
					keys.LSHIFT = FALSE;
					break;
				case SDL_SCANCODE_UP:
					keys.UP = FALSE;
					break;
				case SDL_SCANCODE_DOWN:
					keys.DOWN = FALSE;
					break;
				case SDL_SCANCODE_LEFT:
					keys.LEFT = FALSE;
					break;
				case SDL_SCANCODE_RIGHT:
					keys.RIGHT = FALSE;
					break;
				}
				break;
			}
		}
		move_camera();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		object_draw(&triangle);
		conscr_render();
		context_swap();
	}

	// clean up
	mesh_destroy(&mesh);
	shader_destroy(&shader_program);
	context_destroy();
	conscr_destroy();
	return 0;
}
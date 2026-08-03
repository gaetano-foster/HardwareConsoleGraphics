#define _CRT_SECURE_NO_WARNINGS
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
capture_input()
{
	// use GetAsyncKeyState for input instead of built in SDL inputs
	// so it still works when the window closes
	keys.W = GetAsyncKeyState('W') & 0x8000;
	keys.A = GetAsyncKeyState('A') & 0x8000;
	keys.S = GetAsyncKeyState('S') & 0x8000;
	keys.D = GetAsyncKeyState('D') & 0x8000;
	keys.SPACE = GetAsyncKeyState(' ') & 0x8000;
	keys.LSHIFT = GetAsyncKeyState(VK_LSHIFT) & 0x8000;
	keys.UP = GetAsyncKeyState(VK_UP) & 0x8000;
	keys.LEFT = GetAsyncKeyState(VK_LEFT) & 0x8000;
	keys.DOWN = GetAsyncKeyState(VK_DOWN) & 0x8000;
	keys.RIGHT = GetAsyncKeyState(VK_RIGHT) & 0x8000;
}

void
move_camera()
{
	vec3 cam_angle;
	camera_euler(cam_angle);
	float pitch = cam_angle[0];
	float yaw = cam_angle[1];
	float roll = cam_angle[2];

	if (keys.W)
		camera_translatew((vec3) { SPEED * cosf(yaw), 0, SPEED * sinf(yaw) });
	if (keys.S)
		camera_translatew((vec3) { SPEED * -cosf(yaw), 0, SPEED * -sinf(yaw) });
	if (keys.A)
		camera_translatew((vec3) { SPEED * sinf(yaw), 0, SPEED * -cosf(yaw) });
	if (keys.D)
		camera_translatew((vec3) { SPEED * -sinf(yaw), 0, SPEED * cosf(yaw) });
	if (keys.SPACE)
		camera_translatew((vec3) { 0, SPEED, 0 });
	if (keys.LSHIFT)
		camera_translatew((vec3) { 0, -SPEED, 0 });
	if (keys.UP)
		camera_rotate_rad((vec3) { SENS, 0, 0 });
	if (keys.DOWN)
		camera_rotate_rad((vec3) { -SENS, 0, 0 });
	if (keys.LEFT)
		camera_rotate_rad((vec3) { 0, -SENS, 0 });
	if (keys.RIGHT)
		camera_rotate_rad((vec3) { 0, SENS, 0 });

	if (pitch > glm_rad(89.9)) camera_setrot_rad((vec3) { glm_rad(89.9), yaw, roll });
	if (pitch < glm_rad(-89.9)) camera_setrot_rad((vec3) { glm_rad(-89.9), yaw, roll });
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
	conscr_enablehud();

	shader_t shader_program;
	EXPECT(shader_compile(&shader_program, "vert.glsl", "frag.glsl"));

	// create mesh from vertices
	mesh_t mesh;
	EXPECT(mesh_load(&mesh, "teapot.obj"));

	object_t teapot = {
		.mesh = &mesh,
		.shader = &shader_program
	};
	object_init(&teapot);
	object_setpos(&teapot, (vec3) { 0.0f, 0.0f, -1.0f });

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
	glEnable(GL_DEPTH_TEST); 

	while (running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT: 
				running = FALSE;
				break;
			}
		}
		capture_input();
		move_camera();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		object_draw(&teapot);
		conscr_renderci();
		context_swap();
	}

	// clean up
	mesh_destroy(&mesh);
	shader_destroy(&shader_program);
	context_destroy();
	conscr_destroy();
	return 0;
}
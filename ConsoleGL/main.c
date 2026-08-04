#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <glad/glad.h>
#include <SDL.h>
#include <Windows.h>
#include <cglm/cglm.h>
#include "utils.h"
#include "mesh.h"
#include "conscr.h"
#include "context.h"
#include "shader.h"
#include "object.h"
#include "camera.h"

#define SPEED		(5)
#define SENS		(5)

struct {
	struct {
		BOOL W, A, S, D, SPACE, LSHIFT, UP, DOWN, LEFT, RIGHT;
	} keys;
	struct {
		SDL_Event event;
		BOOL running;
		clock_t last_time;
		clock_t timer;
		INT32 target_fps;
		double time_per_tick;
		double delta; 
		double delta_time;
		INT32 frames;
		INT32 fps;
	} loop;
	object_t teapot;
} state;

void
capture_input()
{
	// use GetAsyncKeyState for input instead of built in SDL inputs
	// so it still works when the window closes
	state.keys.W = GetAsyncKeyState('W') & 0x8000;
	state.keys.A = GetAsyncKeyState('A') & 0x8000;
	state.keys.S = GetAsyncKeyState('S') & 0x8000;
	state.keys.D = GetAsyncKeyState('D') & 0x8000;
	state.keys.SPACE = GetAsyncKeyState(' ') & 0x8000;
	state.keys.LSHIFT = GetAsyncKeyState(VK_LSHIFT) & 0x8000;
	state.keys.UP = GetAsyncKeyState(VK_UP) & 0x8000;
	state.keys.LEFT = GetAsyncKeyState(VK_LEFT) & 0x8000;
	state.keys.DOWN = GetAsyncKeyState(VK_DOWN) & 0x8000;
	state.keys.RIGHT = GetAsyncKeyState(VK_RIGHT) & 0x8000;
}

void
move_camera()
{
	vec3 cam_angle;
	camera_euler(cam_angle);
	float pitch = cam_angle[0];
	float yaw = cam_angle[1];
	float roll = cam_angle[2];
	float dt = state.loop.delta_time;

	if (state.keys.W)
		camera_translatew((vec3) { dt * SPEED * cosf(yaw), 0, dt * SPEED * sinf(yaw) });
	if (state.keys.S)
		camera_translatew((vec3) { dt * SPEED * -cosf(yaw), 0, dt * SPEED * -sinf(yaw) });
	if (state.keys.A)
		camera_translatew((vec3) { dt * SPEED * sinf(yaw), 0, dt * SPEED * -cosf(yaw) });
	if (state.keys.D)
		camera_translatew((vec3) { dt * SPEED * -sinf(yaw), 0, dt * SPEED * cosf(yaw) });
	if (state.keys.SPACE)
		camera_translatew((vec3) { 0, dt * SPEED, 0 });
	if (state.keys.LSHIFT)
		camera_translatew((vec3) { 0, dt * -SPEED, 0 });
	if (state.keys.UP)
		camera_rotate_rad((vec3) { dt * SENS, 0, 0 });
	if (state.keys.DOWN)
		camera_rotate_rad((vec3) { dt * -SENS, 0, 0 });
	if (state.keys.LEFT)
		camera_rotate_rad((vec3) { 0, dt * -SENS, 0 });
	if (state.keys.RIGHT)
		camera_rotate_rad((vec3) { 0, dt * SENS, 0 });

	camera_euler(cam_angle);
	pitch = cam_angle[0];
	yaw = cam_angle[1];
	roll = cam_angle[2];

	if (pitch > glm_rad(89.9)) camera_setrot_rad((vec3) { glm_rad(89.9), yaw, roll });
	if (pitch < glm_rad(-89.9)) camera_setrot_rad((vec3) { glm_rad(-89.9), yaw, roll });
}

void
init()
{
	// initialize screen and opengl context
	EXPECT(context_init(S_WIDTH, S_HEIGHT));
	EXPECT(conscr_init());
	// build teapot object
	EXPECT(shader_compile(&state.teapot.shader, "vert.glsl", "frag.glsl"));
	EXPECT(mesh_load(&state.teapot.mesh, "teapot.obj"));
	object_init(&state.teapot);
	object_setpos(&state.teapot, (vec3) { 0.0f, 0.0f, -1.0f });
	// configure camera
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
	// initialize loop variables
	state.loop.last_time = clock();
	state.loop.timer = 0;
	state.loop.target_fps = 72;
	state.loop.time_per_tick = CLOCKS_PER_SEC / state.loop.target_fps;
	state.loop.delta = 0; 
	state.loop.delta_time = (double)1 / (double)state.loop.target_fps;
	state.loop.frames = 0;
	state.loop.fps = state.loop.target_fps;
}

void
tick()
{
	capture_input();
	move_camera();
}

void
render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	object_draw(&state.teapot);
	conscr_renderci();
	context_swap();
}

void
run()
{
	state.loop.running = TRUE;
	while (state.loop.running) {
		while (SDL_PollEvent(&state.loop.event)) state.loop.running = state.loop.event.type != SDL_QUIT;

		clock_t now = clock();
		state.loop.delta += (now - state.loop.last_time) / state.loop.time_per_tick;
		state.loop.timer += now - state.loop.last_time;
		state.loop.last_time = now;

		if (state.loop.delta >= 1) {
			tick();
			render();
			state.loop.delta--;
			state.loop.frames++;
			if (state.loop.timer >= CLOCKS_PER_SEC) {
				state.loop.fps = state.loop.frames;
				state.loop.delta_time = (double)(1 / (double)state.loop.fps);
				state.loop.timer = 0;
				state.loop.frames = 0;
				CONSCR_HUD_FMT("FPS: %d", state.loop.fps);
			}
		}
	}
}

void
cleanup()
{
	mesh_destroy(state.teapot.mesh);
	shader_destroy(state.teapot.shader);
	context_destroy();
	conscr_destroy();
}

int
main(int argc,
	 char **argv)
{
	init();
	run();
	cleanup();
	return 0;
}
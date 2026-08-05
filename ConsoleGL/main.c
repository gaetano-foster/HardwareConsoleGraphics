#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
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
#include "render_target.h"

#define SPEED		(5)
#define SENS		(5)

struct {
	struct {
		BOOL W, A, S, D, SPACE, LSHIFT, UP, DOWN, LEFT, RIGHT, ESC;
	} keys;
	struct {
		SDL_Event event;
		BOOL running;
		LARGE_INTEGER last_time;
		LARGE_INTEGER timer;
		LARGE_INTEGER freq;
		INT32 target_tps;
		double time_per_tick;
		double delta; 
		double delta_time;
		INT32 ticks;
		INT32 frames;
		INT32 tps;
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
	state.keys.ESC = GetAsyncKeyState(VK_ESCAPE) & 0x8000;
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
	render_target_init(S_WIDTH, S_HEIGHT);
	// build teapot object
	EXPECT(state.teapot.shader = malloc(sizeof(shader_t)));
	EXPECT(state.teapot.mesh = malloc(sizeof(mesh_t)));
	EXPECT(shader_compile(state.teapot.shader, "vert.glsl", "frag.glsl"));
	EXPECT(mesh_load(state.teapot.mesh, "teapot.obj"));
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
	QueryPerformanceFrequency(&state.loop.freq);
	QueryPerformanceCounter(&state.loop.last_time);
	state.loop.timer.QuadPart = 0;
	state.loop.target_tps = 144;
	state.loop.time_per_tick = (double)state.loop.freq.QuadPart / state.loop.target_tps;
	state.loop.delta = 0; 
	state.loop.delta_time = 1.0 / state.loop.target_tps;
	state.loop.frames = 0;
	state.loop.fps = 0;
	state.loop.tps = state.loop.target_tps;
}

void
tick()
{
	capture_input();
	move_camera();

	if (state.keys.ESC) state.loop.running = FALSE;
}

void
render()
{
	conscr_sethud(""); // reset hud between frames
	render_target_bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	object_draw(&state.teapot);
	conscr_render();
	CONSCR_HUD_CAT("FPS: %d               \n", state.loop.fps);
	conscr_renderhud();
	context_swap();
}

void
run()
{
	state.loop.running = TRUE;

	while (state.loop.running) {
		while (SDL_PollEvent(&state.loop.event)) state.loop.running = state.loop.event.type != SDL_QUIT;

		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);

		LONGLONG elapsed = now.QuadPart - state.loop.last_time.QuadPart;
		state.loop.last_time = now;
		state.loop.delta_time = 1.0 / (double)state.loop.tps;
		state.loop.delta += (double)elapsed / state.loop.time_per_tick;
		state.loop.timer.QuadPart += elapsed;

		// lock tickrate
		while (state.loop.delta >= 1.0) {
			tick();
			state.loop.delta--;
			state.loop.ticks++;
		}

		render();
		state.loop.frames++;

		if (state.loop.timer.QuadPart >= state.loop.freq.QuadPart) {
			state.loop.fps = state.loop.frames;
			state.loop.tps = state.loop.ticks;

			state.loop.frames = 0;
			state.loop.ticks = 0;
			state.loop.timer.QuadPart -= state.loop.freq.QuadPart;
		}
	}
}

void
cleanup()
{
	mesh_destroy(*state.teapot.mesh);
	shader_destroy(*state.teapot.shader);
	free(state.teapot.shader);
	free(state.teapot.mesh);
	context_destroy();
	conscr_destroy();
	render_target_cleanup();
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
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

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = FALSE;
			}
		}
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		object_rotate(&triangle, 0.1f, (vec3) { 0.0f, 0.0f, 1.0f });
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
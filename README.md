# Hardware Accelerated Console Graphics

## Overview
A C framework for rendering hardware-accelerated graphics to the Windows console. It comes out of the box with a demo that allows the user to move the camera around a 3D textured block and the renderer is fully configurable with "cmd_render.cfg".

## Build Instructions
The framework is written entirely in C and compiled using MSVC. Simply open the solution and build in Visual Studio 2022 v17.x.x or later. 

**WINDOWS ONLY!! WINDOWS API IS USED EXTENSIVELY!!**

**Note that it only builds a 64 bit executable, Win32/x86 is not supported.**

## Configuration (cmd_render.cfg)
The config file sets up global variables used by the renderer. Options include:

`columns: <0-255>` sets the number of columns of glyphs, AKA the width of the console.

`rows: <0-255>` sets the number of rows of glyphs, AKA the height of the console.

`width: <0-255>` sets the width of the font in pixels.

`height: <0-255>` sets the height of the font in pixels.

`hud: <0 or 1>` defines whether the hud message should be displayed (used for FPS in the sample program).

`renderer: <0 or 1>` sets the rendering mode. 0 = true color and 1 = ascii.

`mask: <0x00-0xF0>` sets the bit mask applied to each color channel in true color mode. 0xF0 discards the 4 least significant bits of each channel, reducing the number of possible colors and improving rendering performance.

`palette: "<string>"` sets the characters to be used by the ascii renderer.

`blue/green/red/intense: <0-255>` sets the threshold for when a pixel should have its corresponding color attribute.

## Dependencies

* [SDL2](https://github.com/libsdl-org/SDL)
* [assimp](https://github.com/assimp/assimp)
* [cglm](https://github.com/recp/cglm)
* [glad-3.3](https://glad.dav1d.de/)
* [stb_image](https://github.com/nothings/stb)

## Usage
Documentation is spread throughout the code in comments (mostly in the header files) for how the framework is to be used by the user. Here is a brief overview of how this framework is meant to be used:

```c
/// --snip--
/// Note: error checking is omitted for clarity
/// Initialization:
object_t teapot;
configure("cmd_render.cfg"); // set the global configuration variables
conscr_init(); // open the command line window and initialize opengl context
// initialize teapot
teapot = {
  .mesh = mesh_load("res/teapot.obj"),
  .shader = shader_compile(VERT_SOURCE, FRAG_SOURCE),
  .texture = texture_load("res/teapot.jpg")
};
object_init(&teapot);
object_setpos(&teapot, (vec3) { 0.0f, 0.0f, -1.0f });
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
/// --snip--
/// Render Loop:
render_target_bind();
// draw
glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
object_draw(&ube);
conscr_render();
conscr_renderhud();
// swap buffers
conscr_swap();
/// --snip--
/// Clean Up:
mesh_cleanup(cube.mesh);
shader_cleanup(cube.shader);
texture_cleanup(cube.texture);
conscr_destroy();
```



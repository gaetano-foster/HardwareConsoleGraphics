#ifndef __CAMERA_H__
#define __CAMERA_H__
#include <cglm/cglm.h>
#include <cglm/mat4.h>

/*
Represents user-defined settings for the global camera

Fields:
	- pos/x, y, z: Anonymous union that represents positional data
	- euler/pitch, yaw, roll: Anonymous union that represents 
	  rotational data
	- fov: Field of view in degrees
*/
struct camera_config_t {
	// view configuration
	union {
		vec3 pos;
		struct {
			float x;
			float y;
			float z;
		};
	};
	union {
		vec3 euler;
		struct {
			float pitch;
			float yaw;
			float roll;
		};
	};
	// projection configuration
	float fov;
	float aspect;
};

/*
Initializes the camera based on caller-defined settings
*/
void
camera_init(struct camera_config_t settings);

////
/// TRANSFORM FUNCTIONS
//

/*
Sets the absolute position of the camera
*/
void
camera_setpos(vec3 pos);

/*
Moves the camera in world space
*/
void
camera_translatew(vec3 dir);

/*
Moves the camera in local space
*/
void
camera_translatel(vec3 dir);

/*
Default translation function (local space)
Equivalent to camera_translatel
*/
void
camera_translate(vec3 dir);

/*
Sets the absolute rotation of the camera

Parameters:
	- vec3 euler: specified euler angles in radians
*/
void
camera_setrot_rad(vec3 euler);

/*
Sets the absolute rotation of the camera

Parameters:
	- vec3 euler: specified euler angles in degrees
*/
void
camera_setrot_deg(vec3 euler);

/*
Rotates the camera in local space

Parameters:
	- vec3 euler: specified euler angles in radians
*/
void 
camera_rotate_rad(vec3 euler);

/*
Rotates the camera in local space

Parameters:
	- vec3 euler: specified euler angles in degrees
*/
void 
camera_rotate_deg(vec3 euler);

////
/// GETTER FUNCTIONS 
//

/*
Gets the view matrix of the global camera

Parameters:
	- mat4 dest: the output mat4
*/
void
camera_view(mat4 dest);

/*
Gets the projection matrix of the global camera

Parameters:
	- mat4 dest: the output mat4
*/
void
camera_proj(mat4 dest);

/*
Gets the forward vector of the global camera

Parameters:
	- vec3 dest: the output vec3
*/
void
camera_forward(vec3 dest);

/*
Gets the up vector of the global camera

Parameters:
	- vec3 dest: the output vec3
*/
void
camera_up(vec3 dest);

/*
Gets the right vector of the global camera

Parameters:
	- vec3 dest: the output vec3
*/
void
camera_right(vec3 dest);

/*
Gets the position vector of the global camera

Parameters:
	- vec3 dest: the output vec3
*/
void
camera_pos(vec3 dest);

/*
Gets the rotation vector of the global camera

Parameters:
	- vec3 dest: the output vec3
*/
void
camera_euler(vec3 dest);

/*
Returns the FOV of the camera
*/
float
camera_fov();

#endif
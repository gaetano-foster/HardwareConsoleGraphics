#ifndef __OBJECT_H__
#define __OBJECT_H__
#include <cglm/cglm.h>
#include "mesh.h"
#include "shader.h"
#include "texture.h"

/*
Represents a graphical object

Fields:
	- transform: object transformation matrix
	- mesh/shader: pointers must point to valid
	  memory, not owned by object
*/
typedef struct _object_t {
	mat4 transform;
	mesh_t *mesh;
	shader_t *shader;
	texture_t *texture;
} object_t;

/* 
Initializes the transform of object_t instance
*/
void
object_init(object_t *object);

/*
Uses object->shader and draws object->mesh to OpenGL frame buffer
*/
void
object_draw(object_t *object);

////
/// TRANSFORM FUNCTIONS
//

/*
Moves the object in world space 
*/
void 
object_translatew(object_t *object, 
	vec3 dir);

/*
Rotates the object in world space

Parameters:
	- float angle: angle in radians
	- vec3 axis: axis to rotate around,
	  must be normalized
*/
void 
object_rotatew(object_t *object, 
	float angle, vec3 axis);

/*
Moves the object in local space 
*/
void 
object_translatel(object_t *object, 
	vec3 dir);

/*
Rotates the object in local space

Parameters:
	- float angle: angle in radians
	- vec3 axis: axis to rotate around,
	  must be normalized
*/
void 
object_rotatel(object_t *object,
	float angle, vec3 axis);

/*
Default translation function (world space)
Equivalent to object_translatew
*/
void 
object_translate(object_t *object, 
	vec3 dir);

/*
Default rotation function
Equivalent to object_rotatel

Parameters:
	- float angle: angle in radians
	- vec3 axis: axis to rotate around,
	  must be normalized
*/
void 
object_rotate(object_t *object, 
	float angle, vec3 axis);

/*
Scales object uniformly in local space
*/
void
object_scale(object_t *object,
	float scale);

/*
Sets absolute position to specified position
*/
void 
object_setpos(object_t *object, 
	vec3 pos);

#endif
#ifndef __OBJECT_H__
#define __OBJECT_H__

typedef struct _object_t {
	mat4 transform;
	mesh_t *mesh;
	shaderprog_t *shader;
} object_t;

/* Initializes an instance of object_t
 Parameters:
	- object_t *object:  pointer to a previously allocated object
*/
void
object_init(object_t *object);

// frees memory associated with object
void 
object_destroy(object_t *object);

void 
object_translate(object_t *, vec3);

void 
object_rotate(object_t *, vec3);

#endif
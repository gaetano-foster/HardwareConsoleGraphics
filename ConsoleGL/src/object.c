#include <cglm/cglm.h>
#include <cglm/struct.h>
#include <glad/glad.h>
#include <Windows.h>
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "object.h"
#include "camera.h"

void
object_init(object_t *object)
{
	glm_mat4_identity(object->transform);
}

void
object_draw(object_t *object)
{
	mat4 proj, view;
	camera_proj(proj);
	camera_view(view);

	shader_use(object->shader);
	texture_bind(object->texture);
	glUniformMatrix4fv(object->shader->proj_loc, 1, GL_FALSE, (float*)proj);
	glUniformMatrix4fv(object->shader->view_loc, 1, GL_FALSE, (float*)view);
	glUniformMatrix4fv(object->shader->model_loc, 1, GL_FALSE, (float*)object->transform);
	mesh_draw(object->mesh);
}

////
/// TRANSFORMATION FUNCTIONS
//

void 
object_translatew(object_t *object,
	vec3 dir)
{
	mat4 temp;
	glm_mat4_identity(temp);
	glm_translate(temp, dir);
	glm_mat4_mul(temp, object->transform, object->transform);
}

void 
object_rotatew(object_t *object,
	float angle, vec3 axis)
{
	mat4 temp;
	glm_mat4_identity(temp);
	glm_rotate(temp, angle, axis);
	glm_mat4_mul(temp, object->transform, object->transform);
}

void 
object_translatel(object_t *object,
	vec3 dir)
{
	glm_translate(object->transform, dir);
}

void 
object_rotatel(object_t *object,
	float angle, vec3 axis)
{
	glm_rotate(object->transform, angle, axis);
}

void 
object_translate(object_t *object,
	vec3 dir)
{
	object_translatew(object, dir);
}

void 
object_rotate(object_t *object,
	float angle, vec3 axis)
{
	object_rotatel(object, angle, axis);
}

void
object_scale(object_t *object,
	float scale)
{
	glm_mat4_scale(object->transform, scale);
}

void 
object_setpos(object_t *object,
	vec3 pos)
{
	glm_translate_to(object->transform, pos, object->transform);
}
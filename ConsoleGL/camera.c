#include <cglm/cglm.h>
#include <cglm/mat4.h>
#include "camera.h"

static struct camera_t {
	// authoritative state
	union {
		vec3 pos;
		struct {
			float x;
			float y;
			float z;
		};
	};
	union {
		vec3 euler; // radians
		struct {
			float pitch;
			float yaw;
			float roll;
		};
	};
	float fov; // radians
	float near;
	float far;
	float aspect;
	// derived state
	vec3 forward;
	vec3 right;
	vec3 up;
	// matrices
	mat4 view;
	mat4 proj;
} camera;

static inline void
update_proj()
{
	glm_perspective(camera.fov, camera.aspect, camera.near, camera.far, camera.proj);
}

static inline void
update_view()
{
	vec3 world_up = {0.0f, 1.0f, 0.0f};
	vec3 target;

	camera.forward[0] = cosf(camera.pitch) * cosf(camera.yaw);
	camera.forward[1] = sinf(camera.pitch);
	camera.forward[2] = cosf(camera.pitch) * sinf(camera.yaw);
	glm_vec3_normalize(camera.forward);

	glm_vec3_cross(camera.forward, world_up, camera.right);
	glm_vec3_normalize(camera.right);

	glm_vec3_cross(camera.right, camera.forward, camera.up);

	glm_vec3_add(camera.pos, camera.forward, target);
	glm_lookat(camera.pos, target, camera.up, camera.view);
}

void
camera_init(struct camera_config_t settings)
{
	// copy configuration into camera
    glm_vec3_copy(settings.pos, camera.pos);
	glm_vec3_copy(settings.euler, camera.euler);
	camera.fov = glm_rad(settings.fov); // camera.fov stores radians, while settings uses degrees for ease of use by the caller
	camera.near = 0.1f;
	camera.far = 1000.0f;
	camera.aspect = settings.aspect;

	// update matrices
	update_proj();
	update_view();
}

////
/// TRANSFORM FUNCTIONS
//

void
camera_setpos(vec3 pos)
{
	glm_vec3_copy(pos, camera.pos);
	update_view();
}

void
camera_translatew(vec3 dir)
{
	glm_vec3_add(dir, camera.pos, camera.pos);
	update_view();
}

void
camera_translatel(vec3 dir)
{
	vec3 delta = GLM_VEC3_ZERO_INIT;

	glm_vec3_muladds(camera.right,   dir[0], delta);
	glm_vec3_muladds(camera.up,      dir[1], delta);
	glm_vec3_muladds(camera.forward, dir[2], delta);

	camera_translatew(delta);
}

void
camera_translate(vec3 dir)
{
	camera_translatel(dir);
}

void
camera_setrot_rad(vec3 euler)
{
	glm_vec3_copy(euler, camera.euler);
	update_view();
}

void
camera_setrot_deg(vec3 euler)
{
	camera_setrot_rad((vec3) { glm_rad(euler[0]), glm_rad(euler[1]), glm_rad(euler[2]) });
}

void 
camera_rotate_rad(vec3 euler)
{
	glm_vec3_add(euler, camera.euler, camera.euler);
	update_view();
}

void 
camera_rotate_deg(vec3 euler)
{
	camera_rotate_rad((vec3) { glm_rad(euler[0]), glm_rad(euler[1]), glm_rad(euler[2]) });
}

////
/// GETTER FUNCTIONS
//

void
camera_view(mat4 dest)
{
	glm_mat4_copy(camera.view, dest);
}

void
camera_proj(mat4 dest)
{
	glm_mat4_copy(camera.proj, dest);
}

void
camera_forward(vec3 dest)
{
	glm_vec3_copy(camera.forward, dest);
}

void
camera_up(vec3 dest)
{
	glm_vec3_copy(camera.up, dest);
}

void
camera_right(vec3 dest)
{
	glm_vec3_copy(camera.right, dest);
}

void
camera_pos(vec3 dest)
{
	glm_vec3_copy(camera.pos, dest);
}

void
camera_euler(vec3 dest)
{
	glm_vec3_copy(camera.euler, dest);
}

float
camera_fov()
{
	return camera.fov;
}
// Camera.h
#pragma once
#include <cmath> // for sin,cos

#include <Eigen/Dense>

#include "Constants.h" // for CameraMovement
#include "Geometry.h"

static const float YAW = -90.0f;
static const float SPEED = 2.5f;
static const float SENSITIVITY = 0.05f;
static const float PITCH = 0.0f;
static const float FOVY = 90.0f;
static const float Z_NEAR = 0.1f;
static const float Z_FAR = 10.0f;
// default eye
static const vec3 DEFAULT_EYE = vec3(0, 0, 2);
static const vec3 DEFAULT_AT = vec3(0, 0, 0);
static const vec3 DEFAULT_UP = vec3(0, 1, 0);
/* To specify a camera, we need to have 12 parameters - eye, and three vectors, u,v, and n, where u-->right/x, v-->y/up, and n-->z/backward. Notice all these values are in world coordinates. Since this is complicated, we'll let the user specify slightly different values - eye, at, and "up", where "up" is a general direction. We set n to be -(at-eye). right/u will be the cross product of "up" \cross n. and v will actually be calculated by taking n \cross u */

/* There are several camera movements:
* https://learnwebgl.brown37.net/07_cameras/camera_movement.html
1 - dragging
2 - panning 
*/

class Camera {
public:
	// constructor
	mat4 view_transform_; // view transformation
	mat4 projection_;
	// Extrinsic parameters
	vec3 eye_; // camera position (world space)
	vec3 at_; // camera position (world space)
	vec3 up_; // world positive up (world space) keep the camera's roll stable and ensure it doesn't start spinning around its forward axis.
	// not suppose to directly set these
	//vec3 u_; // positive x (world space)
	//vec3 v_; // positive y (world space)
	//vec3 n_; // positive z (world space)
	float yaw;
	float pitch_;
	// Intrinsic
	
	float aspect_;
	float fovy_;
	float z_near_;
	float z_far_;

	// Options
	float movement_speed;
	float sensitivity_;

	// Functions
	Camera(vec3 eye = DEFAULT_EYE, vec3 at = DEFAULT_AT, vec3 up = DEFAULT_UP);
	void SetSensitivity(float sensitivity);
	void Reset();
	mat4 LookAt(const vec3& eye, const vec3& at, const vec3& up);
	mat4 GetViewTransform() const { return view_transform_; };
	mat4 GetProjectionTransform() const { return projection_; };
	bool IsPerspectiveProjection() const { return is_perspective_; }
	// Projections
	void SetOrtho(float canvas_right, float canvas_top, float z_near, float z_far);
	void SetPerspective(float aspect, float fovy, float zNear, float zFar);
	// Camera Movement
	void Translate(const vec3& translation);
	void Translate(CameraMovement direction, float delta_time);
	void Pan(float x_offset, float y_offset); // move eye and at along u and v
	void Zoom(float offset); // change y-field of view
	void Dolly(float offset); // move eye and at along n
	void Orbit(float x_offset, float y_offset); // rotate eye circularly around at
	void GetCanvasShape(float& top, float& right) const;
	vec3 GetForward() const;
	void OrbitLeft();
	void OrbitRight();
	void TiltAndYaw(float x_offset, float y_offset); // Change at_ but not eye_
	// Input
	void HandleWindowReshape(int new_width, int new_height);

private:
	bool is_perspective_;
};

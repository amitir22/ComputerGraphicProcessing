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
static const float FOV = 45.0f;
static const float Z_NEAR = 0.1f;
static const float Z_FAR = 100.0f;


class Camera {
public:
	// constructor
	mat4 view_transform; // view transformation
	mat4 projection;
	// Position vectors
	vec3 eye; // camera position
	vec3 forward; // negative z
	vec3 up; // positive y
	vec3 world_up; // positive y
	vec3 right; // positive x
	// Angles
	float yaw;
	float pitch;
	// Options
	float aspect;
	float fov;
	float movement_speed;
	float mouse_sensitivity;

	// other
	float z_near_;
	float z_far_;
	float right_; 
	float top_; 


	// Functions
	Camera(vec3 eye = vec3(0, 0, 2), vec3 at = vec3(0, 0, 0), vec3 up = vec3(0, 1, 0));

	mat4 LookAt(const vec3& eye, const vec3& at, const vec3& up);
	mat4 GetViewTransform();
	mat4 GetProjectionTransform() { return projection; };
	bool IsPerspectiveProjection() const { return is_perspective_; }
	// Projections
	void SetOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
	void SetPerspective(float fovy, float aspect, float zNear, float zFar);	
	// Transformations
	void Translate(const vec3& translation);
	void RotateLeft();
	void RotateRight();
	void Rotate(const vec3& axis, float angle);
	// Input
	void HandleMouseMovement(float x_offset, float y_offset, bool constrain_pitch=true);
	void HandleMouseScroll(float y_offset);
	void HandleKeyboardInput(int key, float delta_time);
	void UpdateVectors();

private:
	bool is_perspective_;
};

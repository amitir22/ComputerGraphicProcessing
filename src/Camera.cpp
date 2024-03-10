// Camera.cpp
#include "Camera.h"

#include <iostream>

// Constructor implementation
Camera::Camera(vec3 eye, vec3 at, vec3 up) : yaw(YAW), pitch(PITCH), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY) {
	//forward = (at - eye).normalized(); // points to negative z
	//right = up.cross(-forward);  // points to positive x
	Reset();
	//SetOrtho(-1, 1, -1, 1, 0.1f, 10);
}

void Camera::Reset() {
	LookAt(DEFAULT_EYE, DEFAULT_AT, DEFAULT_UP);
	float aspect = float(cg::constants::SCR_WIDTH) / float(cg::constants::SCR_HEIGHT);
	SetPerspective(FOVY, aspect, Z_NEAR, Z_FAR);
}

mat4 Camera::LookAt(const vec3& eye, const vec3& at, const vec3& up)
{
	eye_ = eye;
	up_ = up;
	at_ = at;
	// Compute translation matrix 
	mat4 translation_matrix{ 
		{1, 0, 0, -eye.x()},
		{0, 1, 0, -eye.y()},
		{0, 0, 1, -eye.z()},
		{0, 0, 0, 1 }
		};

	// Compute rotation matrix. 
	n_ = (eye - at).normalized(); // points to positive z, aka forward
	//this->gaze = n;
	u_ = (up_.cross(n_)).normalized(); // points to positive x
	v_ = (n_.cross(u_)).normalized(); // points to positive y, really just normalization of up
	mat4 rotation_matrix{
		{u_.x(), u_.y(), u_.z(), 0.0f},
		{v_.x(), v_.y(), v_.z(), 0.0f},
		{n_.x(), n_.y(), n_.z(), 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};
	view_transform_ = rotation_matrix * translation_matrix;
	return view_transform_;
}

void Camera::SetOrtho(float canvas_right, float canvas_top, float z_near, float z_far)
{
	this->canvas_right_ = canvas_right;
	this->canvas_top_ = canvas_top;
	this->z_near_ = z_near;
	this->z_far_ = z_far;
	this->is_perspective_ = false;
	projection_ = geometry::GetOrthoProjection(-canvas_right, canvas_right, -canvas_top, canvas_top, z_near, z_far);
}

void Camera::SetPerspective(float fovy, float aspect, float z_near, float z_far) {
	this->is_perspective_ = true;
	this->fovy_ = fovy;
	this->aspect_ = aspect;
	this->z_near_ = z_near;
	this->z_far_ = z_far;
	geometry::GetTopAndRight(fovy, aspect, z_near, canvas_top_, canvas_right_);
	projection_ = geometry::GetPerspectiveProjection(fovy, aspect, z_near, z_far);
}

void Camera::Translate(CameraMovement direction, float delta_time)
{
	// https://learnwebgl.brown37.net/07_cameras/camera_movement.html
	float velocity = movement_speed * delta_time;
	if (direction == CameraMovement::FORWARD) // dolly in
		Translate(velocity * -n_);
	else if (direction == CameraMovement::BACKWARD) // dolly out
		Translate(velocity * n_);
	else if (direction == CameraMovement::LEFT) // truck left
		Translate(velocity * -u_);
	else if (direction == CameraMovement::RIGHT) // truck right
		Translate(velocity * u_);
	else if (direction == CameraMovement::UP) // pedestal up
		Translate(velocity * v_);
	else if (direction == CameraMovement::DOWN) // pedestal down
		Translate(velocity * -v_);
}

void Camera::Pan(float x_offset, float y_offset)
{
	float sensitivity = 0.01f;
	vec3 translate_dir = u_ * x_offset + v_ * y_offset;
	translate_dir *= sensitivity;
	Translate(translate_dir);
}

void Camera::Dolly(float offset)
{
	float sensitivity = 0.05f;
	float velocity = sensitivity * offset;
	Translate(velocity * -n_);
}

void Camera::Translate(const vec3& translation)
{
	LookAt(eye_ + translation, at_ + translation, up_);
}

void Camera::Orbit(float x_offset, float y_offset)
{
	float yaw = x_offset * 0.1f; // radians
	float pitch = y_offset * 0.05f; // radians
	vec3 direction = eye_ - at_; // points to negative z
	float radius = direction.norm(); // Keep the camera at the same distance from 'at'
	
	// Horizontal rotation
	vec3 rotated_direction = geometry::RotateVector(direction, up_, yaw);
	
	// Vertical rotation
	vec3 horizontal_axis = (up_.cross(direction)).normalized(); // points to positive x
	rotated_direction = geometry::RotateVector(rotated_direction, horizontal_axis, pitch);
	vec3 new_eye = at_ + rotated_direction.normalized() * radius;	

	// Update camera with the new eye position
	LookAt(new_eye, at_, up_);
}

//void Camera::OrbitLeft()
//{
//	float step_size = 0.1f;
//	vec3 translate_dir = this->eye.cross(this->up);
//	float resize_factor = step_size / translate_dir.norm();
//	float prev_eye_norm = this->eye.norm();
//
//	this->eye += resize_factor * translate_dir;
//	this->forward = -this->eye.normalized();
//
//	// resize this->eye
//	this->eye *= prev_eye_norm / this->eye.norm();
//}
//
//void Camera::OrbitRight()
//{
//	float step_size = 0.1f;
//	vec3 translate_dir = this->eye.cross(this->up);
//	float resize_factor = step_size / translate_dir.norm();
//	float prev_eye_norm = this->eye.norm();
//
//	this->eye -= resize_factor * translate_dir;
//	this->forward = -this->eye.normalized();
//
//	// resize this->eye
//	this->eye *= prev_eye_norm / this->eye.norm();
//}
// Camera.cpp
#include "Camera.h"

#include <iostream>

// Constructor implementation
Camera::Camera(vec3 eye, vec3 at, vec3 up) : yaw(YAW), pitch_(PITCH), movement_speed(SPEED), sensitivity_(SENSITIVITY) {
	Reset();
	//SetOrtho(-1, 1, -1, 1, 0.1f, 10);
}

void Camera::SetSensitivity(float sensitivity)
{
	this->sensitivity_ = sensitivity;
}

void Camera::Reset() {
	pitch_ = PITCH;
	yaw = YAW;
	LookAt(DEFAULT_EYE, DEFAULT_AT, DEFAULT_UP);
	float aspect = float(cg::constants::SCR_WIDTH) / float(cg::constants::SCR_HEIGHT);
	SetPerspective(aspect, FOVY, Z_NEAR, Z_FAR);
}

mat4 Camera::LookAt(const vec3& eye, const vec3& at, const vec3& up)
{
	eye_ = eye;
	at_ = at;
	up_ = up;
	// Compute translation matrix 
	mat4 translation_matrix{ 
		{1, 0, 0, -eye.x()},
		{0, 1, 0, -eye.y()},
		{0, 0, 1, -eye.z()},
		{0, 0, 0, 1 }
		};

	// Compute rotation matrix. 
	vec3 n = (eye - at).normalized(); // points to positive z
	//this->gaze = n;
	vec3 u = (up_.cross(n)).normalized(); // points to positive x
	vec3 v = (n.cross(u)).normalized(); // points to positive y
	mat4 rotation_matrix{
		{u.x(), u.y(), u.z(), 0.0f},
		{v.x(), v.y(), v.z(), 0.0f},
		{n.x(), n.y(), n.z(), 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};
	view_transform_ = rotation_matrix * translation_matrix;
	return view_transform_;
}

void Camera::SetOrtho(float aspect, float fovy, float z_near, float z_far)
{
	this->is_perspective_ = false;
	this->aspect_ = aspect;
	this->fovy_ = fovy;
	this->z_near_ = z_near;
	this->z_far_ = z_far;
	projection_ = geometry::GetOrthoProjection(aspect, fovy, z_near, z_far);
}

void Camera::SetPerspective(float aspect, float fovy, float z_near, float z_far) {
	this->is_perspective_ = true;
	this->fovy_ = fovy;
	this->aspect_ = aspect;
	this->z_near_ = z_near;
	this->z_far_ = z_far;
	projection_ = geometry::GetPerspectiveProjection(aspect, fovy, z_near, z_far);
}

void Camera::Translate(CameraMovement direction, float delta_time)
{
	// https://learnwebgl.brown37.net/07_cameras/camera_movement.html
	float velocity = movement_speed * delta_time;
	vec3 n = (eye_ - at_).normalized(); // points to positive z
	vec3 u = (up_.cross(n)).normalized(); // points to positive x
	vec3 v = (n.cross(u)).normalized(); // points to positive y
	if (direction == CameraMovement::FORWARD) // dolly in
		Translate(velocity * -n);
	else if (direction == CameraMovement::BACKWARD) // dolly out
		Translate(velocity * n);
	else if (direction == CameraMovement::LEFT) // truck left
		Translate(velocity * -u);
	else if (direction == CameraMovement::RIGHT) // truck right
		Translate(velocity * u);
	else if (direction == CameraMovement::UP) // pedestal up
		Translate(velocity * v);
	else if (direction == CameraMovement::DOWN) // pedestal down
		Translate(velocity * -v);
}

void Camera::Pan(float x_offset, float y_offset)
{
	vec3 n = (eye_ - at_).normalized(); // points to positive z
	vec3 u = (up_.cross(n)).normalized(); // points to positive x
	vec3 v = (n.cross(u)).normalized(); // points to positive y
	vec3 translate_dir = u * x_offset + v * y_offset;
	translate_dir *= sensitivity_;
	Translate(translate_dir);
}

void Camera::Zoom(float offset) {
	fovy_ += offset;
	if (fovy_ <= 1.0f)
		fovy_ = 1.0f;
	else if (fovy_ >= 140.0f)
		fovy_ = 140.0f;
	if (is_perspective_)
		SetPerspective(aspect_, fovy_, z_near_, z_far_);
	else
		SetOrtho(aspect_, fovy_, z_near_, z_far_);
}

void Camera::HandleWindowReshape(int new_width, int new_height) {
	aspect_ = float(new_width) / float(new_height);
	if (is_perspective_)
		SetPerspective(aspect_, fovy_, z_near_, z_far_);
	else
		SetOrtho(aspect_, fovy_, z_near_, z_far_);

}

void Camera::Dolly(float offset)
{
	vec3 n = (eye_ - at_).normalized(); // points to positive z
	float velocity = sensitivity_ * offset;
	Translate(velocity * -n);
}

void Camera::Translate(const vec3& translation)
{
	LookAt(eye_ + translation, at_ + translation, up_);
}

void Camera::Orbit(float x_offset, float y_offset)
{
	float yaw = x_offset * sensitivity_; // radians
	float pitch = y_offset * sensitivity_; // radians
	// constrain pitch
	if (pitch_ + pitch > 1.56f) {
		pitch_ = 1.56f;
		pitch = 0;
	}
		
	else if (pitch_ + pitch < -1.56f) {
		pitch_ = -1.56f;
		pitch = 0;
	}
	else
		pitch_ += pitch;
	vec3 direction = eye_ - at_; // points to positive z
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

// view/camera coordinate-space
void Camera::GetCanvasShape(float& top, float& right) const {
	
	if (is_perspective_)
		geometry::GetTopAndRight(aspect_, fovy_, z_near_, top, right);
	// TODO ortho
}

vec3 Camera::GetForward() const
{
	return -(eye_ - at_).normalized(); // points to negative z
}

void Camera::OrbitLeft()
{
	float step_size = 0.1f;
	vec3 translate_dir = this->eye_.cross(this->up_);
	float resize_factor = step_size / translate_dir.norm();
	float prev_eye_norm = this->eye_.norm();

	this->eye_ += resize_factor * translate_dir;

	// resize this->eye
	this->eye_ *= prev_eye_norm / this->eye_.norm();
	this->LookAt(this->eye_, this->at_, this->up_);
}

void Camera::OrbitRight()
{
	float step_size = 0.1f;
	vec3 translate_dir = this->eye_.cross(this->up_);
	float resize_factor = step_size / translate_dir.norm();
	float prev_eye_norm = this->eye_.norm();

	this->eye_ -= resize_factor * translate_dir;

	// resize this->eye
	this->eye_ *= prev_eye_norm / this->eye_.norm();
	this->LookAt(this->eye_, this->at_, this->up_);
}

void Camera::TiltAndYaw(float x_offset, float y_offset) // Change at_ but not eye_
{
	float pitch = y_offset * 0.001f; // radians
	// constrain pitch
	if (pitch_ + pitch > 1.57f) {
		pitch_ = 1.57f;
		pitch = 0;
	}
	else if (pitch_ + pitch < -1.57f) {
		pitch_ = -1.57f;
		pitch = 0;
	}
	else
		pitch_ += pitch;

	vec3 forward = (at_ - eye_); // points to negative z
	vec3 right = (forward.cross(up_)).normalized(); // points to positive x
	vec3 rotated_direction = geometry::RotateVector(forward, right, pitch);
	// now compute for yaw
	float yaw = x_offset * 0.001f; // radians
	rotated_direction = geometry::RotateVector(rotated_direction, up_, yaw);
	vec3 new_at = eye_ + rotated_direction;
	LookAt(eye_, new_at, up_);
}

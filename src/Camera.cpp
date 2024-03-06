// Camera.cpp
#include "Camera.h"

#include <iostream>

// Constructor implementation
Camera::Camera(vec3 eye, vec3 at, vec3 up) : eye(eye), up(up), yaw(YAW), pitch(PITCH),
fov(FOV), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), world_up(0,1,0) {
	gaze = (at - eye).normalized(); // points to negative z
	right = up.cross(-gaze);  // points to positive x
	LookAt(eye, at, up);
	aspect = float(cg::constants::SCR_WIDTH) / float(cg::constants::SCR_HEIGHT);
	z_near_ = Z_NEAR;
	z_far_ = Z_FAR;
	top_ = 0.0f;
	right_ = 0.0f;
	SetPerspective(fov, aspect, Z_NEAR,Z_FAR);
	//SetOrtho(-1, 1, -1, 1, 0.1f, 10);
	UpdateVectors();
}

mat4 Camera::LookAt(const vec3& eye, const vec3& at, const vec3& up)
{
	// Compute translation matrix 
	mat4 translation_matrix{ 
		{1, 0, 0, -eye.x()},
		{0, 1, 0, -eye.y()},
		{0, 0, 1, -eye.z()},
		{0, 0, 0, 1 }
		};

	// Compute rotation matrix. 
	vec3 n = (eye - at).normalized(); // points to positive z, aka forward
	//this->gaze = n;
	vec3 u = (up.cross(n)).normalized(); // points to positive x
	vec3 v = (n.cross(u)).normalized(); // points to positive y, really just normalization of up
	mat4 rotation_matrix{
		{u.x(), u.y(), u.z(), 0.0f},
		{v.x(), v.y(), v.z(), 0.0f},
		{n.x(), n.y(), n.z(), 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};
	view_transform = rotation_matrix * translation_matrix;
	return view_transform;
}

mat4 Camera::GetViewTransform() {
	return LookAt(eye, eye + gaze, up);
}

void Camera::SetOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	this->is_perspective_ = false;
	projection = geometry::getOrthoProjection(left, right, bottom, top, zNear, zFar);
}

void Camera::SetPerspective(float fovy, float aspect, float zNear, float zFar) {
	this->is_perspective_ = true;
	geometry::getTopAndRight(fovy, aspect, zNear, top_, right_);
	projection = geometry::getPerspectiveProjection(fovy, aspect, zNear, zFar);
}

void Camera::HandleMouseMovement(float x_offset, float y_offset, bool constrain_pitch)
{
	x_offset *= mouse_sensitivity;
	y_offset *= mouse_sensitivity;

	yaw += x_offset;
	pitch += y_offset;

	if (constrain_pitch) {
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}
	UpdateVectors();
}

void Camera::HandleMouseScroll(float y_offset)
{
	fov -= y_offset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 70.0f)
		fov = 70.0f;
	std::cout << "fov: " << fov << std::endl;
	SetPerspective(fov, aspect, Z_NEAR, Z_FAR);
}

void Camera::HandleKeyboardInput(int key, float delta_time)
{
	float velocity = movement_speed * delta_time;
	if (key == CameraMovement::FORWARD)
		Translate(velocity * gaze);
	if (key == CameraMovement::BACKWARD)
		Translate(velocity * (-gaze));
	if (key == CameraMovement::LEFT)
		Translate(velocity * (-right));
	if (key == CameraMovement::RIGHT)
		Translate(velocity * right);
	if (key == CameraMovement::UP)
		Translate(velocity * up);
	if (key == CameraMovement::DOWN)
		Translate(velocity * (-up));
	if (key == CameraMovement::ROTATE)
		Rotate();
	
}


void Camera::UpdateVectors() {
	gaze.x() = cos(geometry::radians(yaw)) * cos(geometry::radians(pitch));
	gaze.y() = sin(geometry::radians(pitch));
	gaze.z() = sin(geometry::radians(yaw)) * cos(geometry::radians(pitch));
	gaze.normalize();
	right = (gaze.cross(world_up)).normalized();
	up = (right.cross(gaze)).normalized();
}

void Camera::Translate(const vec3& translation)
{
	eye += translation;
	std::cout << "eye: " << eye.transpose() << std::endl;
} 

void Camera::Rotate()
{
	float step_size = 0.1;
	vec3 translate_dir = this->eye.cross(this->up);
	float resize_factor = step_size / translate_dir.norm();

	this->eye += resize_factor * translate_dir;
	this->gaze = -this->eye.normalized();
}

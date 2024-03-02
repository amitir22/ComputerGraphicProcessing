#include "Camera.h"
#include <cmath>

float radians(float degrees) { return degrees * M_PI / 180; }


// Constructor implementation
Camera::Camera(vec3 eye, vec3 at, vec3 up) : eye(eye), up(up), yaw(YAW), pitch(PITCH),
zoom(ZOOM), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), world_up(0, 1, 0) {
	gaze = normalize(at - eye); // points to negative z
	right = cross(up, -gaze);  // points to positive x
	LookAt(eye, at, up);
	SetOrtho(-1, 1, -1, 1, 0.1, 10);
	//SetPerspective(-1, 1, -1, 1, 0.1, 10);
	UpdateVectors();
}

// compute change-of-basis from world pose to camera pose, keeping in mind that the camera looks towards -z 
// We've used https://www.songho.ca/opengl/gl_camera.html#lookat as a reference
mat4 Camera::LookAt(const vec3& eye, const vec3& at, const vec3& up)
{
	// Compute translation matrix 
	mat4 translation_matrix = mat4(1.0f, 0.0f, 0.0f, 0.0f, // column/row-major
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-eye.x, -eye.y, -eye.z, 1.0f);

	// Compute rotation matrix. 
	vec3 n = normalize(eye - at); // points to positive z, aka forward
	vec3 u = normalize(cross(up, n)); // points to positive x
	vec3 v = normalize(cross(n, u)); // points to positive y, really just normalization of up
	mat4 rotation_matrix = mat4(vec4(u.x, u.y, u.z, 0.0f),
		vec4(v.x, v.y, v.z, 0.0f),
		vec4(n.x, n.y, n.z, 0.0f),
		vec4(0.0f, 0.0f, 0.0f, 1.0f));
	view_transform = rotation_matrix * translation_matrix;
	return view_transform;
}

mat4 Camera::GetCameraTransform() {
	return LookAt(eye, eye + gaze, up);
}


void Camera::SetOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	this->is_perspective_ = false;
	projection = Geometry::getOrthoProjection(left, right, bottom, top, zNear, zFar);
}

void Camera::SetPerspective(float left, float right, float bottom, float top, float zNear, float zFar)
{
	this->is_perspective_ = true;
	projection = Geometry::getPerspectiveProjection(left, right, bottom, top, zNear, zFar);
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

void Camera::HandleKeyboardInput(int key, float delta_time)
{
	float velocity = movement_speed * delta_time;
	if (key == FORWARD)
		Translate(velocity * gaze);
	if (key == BACKWARD)
		Translate(velocity * (-gaze));
	if (key == LEFT)
		Translate(velocity * (-right));
	if (key == RIGHT)
		Translate(velocity * right);
}

void Camera::UpdateVectors() {
	gaze.x = cos(radians(yaw)) * cos(radians(pitch));
	gaze.y = sin(radians(pitch));
	gaze.z = sin(radians(yaw)) * cos(radians(pitch));
	gaze = normalize(gaze);
	right = normalize(cross(gaze, world_up));
	up = normalize(cross(right, gaze));
}

void Camera::Translate(const vec3& translation)
{
	eye += translation;
	cout << eye << endl;
}

#include "StdAfx.h"
#include "Camera.h"
#include <cmath>
#include "constants.h"

float radians(float degrees) { return degrees * M_PI / 180; }


// Constructor implementation
Camera::Camera(vec3 eye, vec3 at, vec3 up) : eye(eye), up(up), yaw(YAW), pitch(PITCH),
zoom(ZOOM), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), worldUp(0,1,0) {
	gaze = normalize(at - eye); // points to negative z
	right = cross(up, -gaze);  // points to positive x
	LookAt(eye, at, up);
	setOrtho(-1, 1, -1, 1, 0.1, 10);
	//setPerspective(-1, 1, -1, 1, 0.1, 10);
	updateVectors();
}

// compute change-of-basis from world pose to camera pose, keeping in mind that the camera looks towards -z 
// We've used https://www.songho.ca/opengl/gl_camera.html#lookat as a reference
mat4 Camera::LookAt(const vec3& eye, const vec3& at, const vec3& up)
{
	// Compute translation matrix 
	mat4 translationMatrix = mat4(1.0f, 0.0f, 0.0f, 0.0f, // column-major order
								  0.0f, 1.0f, 0.0f, 0.0f,
								  0.0f, 0.0f, 1.0f, 0.0f,
								  -eye.x, -eye.y, -eye.z, 1.0f);

	// Compute rotation matrix. 
	vec3 n = normalize(eye - at); // points to positive z, aka forward
	vec3 u = normalize(cross(up, n)); // points to positive x
	vec3 v = normalize(cross(n,u)); // points to positive y, really just normalization of up
	mat4 rotationMatrix = mat4(vec4(u.x, u.y, u.z, 0.0f),
							   vec4(v.x, v.y, v.z, 0.0f),
							   vec4(n.x, n.y, n.z, 0.0f),
							   vec4(0.0f, 0.0f, 0.0f, 1.0f));
	viewTransform = rotationMatrix * translationMatrix;
	return viewTransform;
}

mat4 Camera::getCameraTransformMatrix() {
	return LookAt(eye, eye + gaze, up);
}


void Camera::setOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	m_isPerspective = false;
	projection = Geometry::getOrthoProjection(left, right, bottom, top, zNear, zFar);
}

void Camera::setPerspective(float left, float right, float bottom, float top, float zNear, float zFar)
{
	m_isPerspective = true;
	projection = Geometry::getPerspectiveProjection(left, right, bottom, top, zNear, zFar);	
}

void Camera::handleMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (constrainPitch) {
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}
	updateVectors();

}

void Camera::handleKeyboardInput(int key, float deltaTime)
{
	float velocity = movementSpeed * deltaTime;
	if (key == FORWARD)
		Translate(velocity * gaze);
	if (key == BACKWARD)
		Translate(velocity * (-gaze));
	if (key == LEFT)
		Translate(velocity * (-right));
	if (key == RIGHT)
		Translate(velocity * right);
}

void Camera::updateVectors() {
	gaze.x = cos(radians(yaw)) * cos(radians(pitch));
	gaze.y = sin(radians(pitch));
	gaze.z = sin(radians(yaw)) * cos(radians(pitch));
	gaze = normalize(gaze);
	right = normalize(cross(gaze, worldUp));
	up = normalize(cross(right, gaze));
}

void Camera::handleWindowReshape(int width, int height)
{
	const float PIXELS_PER_UNIT = 400.0;
	float worldWidth = width / PIXELS_PER_UNIT;
	float worldHeight = height / PIXELS_PER_UNIT;

	// Set the viewport to cover the new window
	if (!m_isPerspective) {
		setOrtho(-worldWidth / 2, worldWidth / 2, -worldHeight / 2, worldHeight / 2, 0.1, 10);
	}
	
}

void Camera::Translate(const vec3& translation)
{
	eye += translation;
	cout << "eye " << eye << endl;
}

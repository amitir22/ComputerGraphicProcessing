#include "StdAfx.h"
#include "Camera.h"
#include <cmath>
#include "constants.h"

// Constructor implementation
Camera::Camera(vec3 eye, vec3 at, vec3 up) : eye(eye), up(up), yaw(YAW), pitch(PITCH),
fov(FOV), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), worldUp(0,1,0) {
	gaze = normalize(at - eye); // points to negative z
	right = cross(up, -gaze);  // points to positive x
	LookAt(eye, at, up);
	aspect = float(CG::DEFAULT_WIDTH) / float(CG::DEFAULT_HEIGHT);
	//setOrtho(-1, 1, -1, 1, Z_NEAR, Z_FAR);
	//setPerspective(-1, 1, -1, 1, Z_NEAR, Z_FAR);
	setPerspective(fov, aspect, Z_NEAR,Z_FAR);
	updateVectors();
}

Camera::Camera(const Camera& other) {
	this->viewTransform = other.viewTransform;
	this->projection = other.projection;
	this->eye = other.eye;
	this->gaze = other.gaze;
	this->up = other.up;
	this->worldUp = other.worldUp;
	this->right = other.right;
	this->yaw = other.yaw;
	this->pitch = other.pitch;
	this->fov = other.fov;
	this->movementSpeed = other.movementSpeed;
	this->mouseSensitivity = other.mouseSensitivity;
	this->aspect = other.aspect;
}

// compute change-of-basis from world pose to camera pose, keeping in mind that the camera looks towards -z 
mat4 Camera::LookAt(const vec3& eye, const vec3& at, const vec3& up) {
	 //Compute translation matrix 
	mat4 translationMatrix = mat4(vec4(1.0f, 0.0f, 0.0f, -eye.x), 
								  vec4(0.0f, 1.0f, 0.0f, -eye.y),
								  vec4(0.0f, 0.0f, 1.0f, -eye.z),
								  vec4(0.0f, 0.0f, 0.0f, 1.0f));

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

void Camera::setOrtho(float left, float right, float bottom, float top, float zNear, float zFar){
	m_isPerspective = false;
	projection = Geometry::getOrthoProjection(left, right, bottom, top, zNear, zFar);
}

void Camera::setPerspective(float left, float right, float bottom, float top, float zNear, float zFar) {
	m_isPerspective = true;
	projection = Geometry::getPerspectiveProjection(left, right, bottom, top, zNear, zFar);	
}

void Camera::setPerspective(float fovy, float aspect, float zNear, float zFar) {
	m_isPerspective = true;
	projection = Geometry::getPerspectiveProjection(fovy, aspect, zNear, zFar);
}

void Camera::handleMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;
	cout << "handleMouseMovement: " << "xoffset " << xoffset << " yoffset " << yoffset << endl;

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

void Camera::handleMouseScroll(float yoffset) {
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > FOV)
		fov = FOV;
	cout << "fov " << fov << endl;
	setPerspective(fov, aspect, Z_NEAR, Z_FAR);
}

void Camera::handleKeyboardInput(int key, float deltaTime)
{
	float velocity = movementSpeed * deltaTime;
	if (key == FORWARD)
		translate(velocity * gaze);
	if (key == BACKWARD)
		translate(velocity * (-gaze));
	if (key == LEFT)
		translate(velocity * (-right));
	if (key == RIGHT)
		translate(velocity * right);
	if (key == UP)
		translate(velocity * up);
	if (key == DOWN)
		translate(velocity * (-up));

}

void Camera::updateVectors() {
	gaze.x = cos(Geometry::radians(yaw)) * cos(Geometry::radians(pitch));
	gaze.y = sin(Geometry::radians(pitch));
	gaze.z = sin(Geometry::radians(yaw)) * cos(Geometry::radians(pitch));
	gaze = normalize(gaze);
	right = normalize(cross(gaze, worldUp));
	up = normalize(cross(right, gaze));
}

void Camera::handleWindowReshape(int width, int height)
{
	aspect = float(width) / float(height);
	const float PIXELS_PER_UNIT = 400.0;
	float worldWidth = width / PIXELS_PER_UNIT;
	float worldHeight = height / PIXELS_PER_UNIT;

	// Set the viewport to cover the new window
	if (!m_isPerspective) {
		setOrtho(-worldWidth / 2, worldWidth / 2, -worldHeight / 2, worldHeight / 2, Z_NEAR, Z_FAR);
	}
	else {
		setPerspective(-worldWidth / 2, worldWidth / 2, -worldHeight / 2, worldHeight / 2, Z_NEAR, Z_FAR);
	}
}

void Camera::translate(const vec3& translation)
{
	eye += translation;
	cout << "eye " << eye << endl;
}
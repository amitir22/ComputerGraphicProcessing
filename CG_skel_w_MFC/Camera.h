// Camera.h
#pragma once
#include "mat.h"
#include "vec.h"
#include "Geometry.h"
#include <cmath>
using namespace std;

static const float YAW = -90.0f;
//static const float SPEED = 2.5f;
static const float SPEED = 1.0f;
static const float SENSITIVITY = 0.1f;
static const float PITCH = 0.0f;
static const float ZOOM = 45.0f;


enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera {
public:
	// constructor
	mat4 viewTransform; // view transformation
	mat4 projection;
	// Position vectors
	vec3 eye; // camera position
	vec3 gaze; // negative z
	vec3 up; // positive y
	vec3 worldUp; // positive y
	vec3 right; // positive x
	// Angles
	float yaw; 
	float pitch;
	// Options
	float zoom;
	float movementSpeed;
	float mouseSensitivity;
	

	// Functions
	Camera(vec3 eye = vec3(0, 0, 2), vec3 at = vec3(0, 0, 0), vec3 up = vec3(0, 1, 0));

	mat4 LookAt(const vec3& eye, const vec3& at, const vec3& up);
	mat4 getCameraTransformMatrix();
	// Projections
	void setOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
	void setPerspective(float left, float right, float bottom, float top, float zNear, float zFar);
	void Frustum(float left, float right, float bottom, float top, float zNear, float zFar);
	//mat4 Perspective(float fovy, float aspect, float zNear, float zFar);
	void handleWindowReshape(int width, int height);
	// Functions
	void Translate(const vec3& translation);
	void Rotate(const vec3& axis, float angle);
	void ZoomIn(float zoom);

	void handleMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
	void handleMouseScroll(float yoffset);
	void handleKeyboardInput(int key, float deltaTime);
	void updateVectors();
	bool isPerspectiveProjection() { return m_isPerspective;}
private:
	bool m_isPerspective;
};

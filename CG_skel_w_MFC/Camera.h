#pragma once
#include "gl/glew.h"
#include "Renderer.h"

using namespace std;

class Camera {
	mat4 cTransform; // camera (view) transformation, I think
	mat4 projection;

public:
	void setTransformation(const mat4& transform);
	void LookAt(const vec4& eye, const vec4& at, const vec4& up);
	void Ortho(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar);
	void Frustum(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar);
	mat4 Perspective(const float fovy, const float aspect,
		const float zNear, const float zFar);

};

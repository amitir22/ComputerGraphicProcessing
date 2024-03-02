#pragma once
#include "vec.h"
#include "mat.h"

namespace Geometry {
	mat4 makeRotationMatrix(const vec3& axis, float angle);
	mat4 makeRotationMatrix(float yaw, float pitch, float roll);
	mat4 makeScaleMatrix(const vec3& scale);
	mat4 getOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar);
	mat4 getPerspectiveProjection(float left, float right, float bottom, float top, float zNear, float zFar);
	void Frustum(float left, float right,
		float bottom, float top,
		float zNear, float zFar);
	mat4 getViewPortTransform(int width, int height); // Map NDC to screen-space coordinates
}
#pragma once
#include <Eigen/Dense>

using vec2 = Eigen::Vector2f;
using vec3 = Eigen::Vector3f;
using vec4 = Eigen::Vector4f;
using mat3 = Eigen::Matrix3f;
using mat4 = Eigen::Matrix4f;

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
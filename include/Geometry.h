#pragma once

#include <iostream>

#include <Eigen/Dense>
using vec2i = Eigen::Vector2i;
using vec2 = Eigen::Vector2f;
using vec3 = Eigen::Vector3f;
using vec4 = Eigen::Vector4f;
using mat3 = Eigen::Matrix3f;
using mat4 = Eigen::Matrix4f;

vec2 vec2fFromStream(std::istream& a_stream);
vec3 vec3fFromStream(std::istream& a_stream);


namespace geometry {
	float Radians(float degrees);
	mat4 GetRotationMatrix(const vec3& axis, float angle);
	mat4 GetRotationMatrix(float yaw, float pitch, float roll);
	mat4 GetScaleMatrix(const vec3& scale);
	mat4 GetTranslationMatrix(vec3 translation);
	mat4 GetPerspectiveProjection(float fovy, float aspect, float zNear, float zFar);
	mat4 GetOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar);
	mat4 GetPerspectiveProjection(float left, float right, float bottom, float top, float zNear, float zFar);
    void GetTopAndRight(float fovy, float aspect,float zNear, float &top, float &right);
    mat4 GetViewportTransform(int width, int height); // Map NDC to screen-space coordinates

    mat3 GetNormalTransfrom(const mat4& m); // from modelview

	float FastQuake3InverseSqrt(float num);
} // namespace geometry


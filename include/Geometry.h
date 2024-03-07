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
	float radians(float degrees);
	mat4 makeRotationMatrix(const vec3& axis, float angle);
	mat4 makeRotationMatrix(float yaw, float pitch, float roll);
	mat4 makeScaleMatrix(const vec3& scale);
	mat4 makeTranslationMatrix(vec3 translation);
	mat4 getPerspectiveProjection(float fovy, float aspect, float zNear, float zFar);
	mat4 getOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar);
	mat4 getPerspectiveProjection(float left, float right, float bottom, float top, float zNear, float zFar);
    void getTopAndRight(float fovy, float aspect,float zNear, float &top, float &right);
    mat4 getViewPortTransform(int width, int height); // Map NDC to screen-space coordinates

    mat3 getNormalTransfrom(const mat4& m); // from modelview
} // namespace geometry


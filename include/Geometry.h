#pragma once

#include <iostream>

#include <Eigen/Dense>
using vec2i = Eigen::Vector2i;
using vec2 = Eigen::Vector2f;
using vec3 = Eigen::Vector3f;
using vec4 = Eigen::Vector4f;
using mat3 = Eigen::Matrix3f;
using mat4 = Eigen::Matrix4f;
using matxf = Eigen::MatrixXf;

vec2 vec2fFromStream(std::istream& a_stream);
vec3 vec3fFromStream(std::istream& a_stream);


namespace geometry {

	float Radians(float degrees);
	vec3 RotateVector(const vec3& v, const vec3& axis, float angle);
	mat4 GetRotationMatrix(const vec3& axis, float angle);
	mat4 GetRotationMatrix(float yaw, float pitch, float roll);
	mat4 GetScaleMatrix(const vec3& scale);
	mat4 GetTranslationMatrix(vec3 translation);
	// Projections
	mat4 GetPerspectiveProjection(float aspect, float fovy, float zNear, float zFar);
	mat4 GetPerspectiveProjection(float left, float right, float bottom, float top, float zNear, float zFar);
	mat4 GetOrthoProjection(float aspect, float fovy, float z_near, float z_far);
	mat4 GetOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar);
    void GetTopAndRight(float aspect, float fovy, float zNear, float &top, float &right);
    mat4 GetViewportTransform(int width, int height); // Map NDC to screen-space coordinates
    mat3 GetNormalTransfrom(const mat4& m); // from modelview

	mat3 GetWorldNormalTransform(const mat4& m);

	struct Rectangle {
		Rectangle(const vec2& bottom_left, const vec2& top_right);
		Rectangle();
		Rectangle(float width, float height, const vec2& bottom_left);
		Rectangle(float left_x, float right_x, float bottom_y, float top_y);
		Rectangle(float width, float height);

		float GetLeft() const { return bottom_left_.x(); }
		float GetRight() const { return top_right_.x(); }
		float GetBottom() const { return bottom_left_.y(); }
		float GetTop() const { return top_right_.y(); }

		float GetWidth() const { return top_right_.x() - bottom_left_.x(); }
		float GetHeight() const { return top_right_.y() - bottom_left_.y(); }

		vec2 bottom_left_;
		vec2 top_right_;
	};

	struct Box {
		Box(const vec3& bottom_left_back_, const vec3& top_right_front_);
		Box();
		Box(float width, float height, float depth);
		Box(float width, float height, float depth, const vec3& bottom_left_front);

		float GetLeft() const { return bottom_left_back_.x(); }
		float GetRight() const { return top_right_front_.x(); }
		float GetBottom() const { return bottom_left_back_.y(); }
		float GetTop() const { return top_right_front_.y(); }
		float GetBack() const { return bottom_left_back_.z(); }
		float GetFront() const { return top_right_front_.z(); }

		float GetWidth() const { return top_right_front_.x() - bottom_left_back_.x(); }
		float GetHeight() const { return top_right_front_.y() - bottom_left_back_.y(); }
		float GetDepth() const { return top_right_front_.z() - bottom_left_back_.z(); }

		vec3 bottom_left_back_;
		vec3 top_right_front_;

	}; // Box
	float FastQuake3InverseSqrt(float num);

} // namespace geometry


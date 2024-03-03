#include "Geometry.h"
#include <cmath> // for sin,cos

#define M_PI 3.14159265358979323846f
float geometry::radians(float degrees) { return degrees * M_PI / 180; }

// We call this orthographic projection, but we don't really project, since we don't want to lose depth information.
mat4 geometry::getOrthoProjection(float left, float right, float bottom, float top,
								  float zNear, float zFar)
{
	// translate center of orthographic viewing volume to origin (0,0,0)
	mat4 translationMatrix{
		{1, 0, 0, -(right + left) / 2},
		{0, 1, 0, -(top + bottom) / 2},
		{0, 0, 1, -(-zFar + (-zNear)) / 2},
		{0, 0, 0, 1}
	};
	// scale to make the orthographic viewing volume into a unit cube
	mat4 scaleMatrix{
		{2 / (right - left), 0, 0, 0},
		{0, 2 / (top - bottom), 0, 0},
		{0, 0, 2 / (zFar - zNear), 0},
		{0, 0, 0, 1}
	};
	// The result looks like:
	// 2/(right-left) 0 0 -(right+left)/(right-left)
	// 0 2/(top-bottom) 0 -(top+bottom)/(top-bottom)
	// 0 0 2/(zFar-zNear) -(-zFar+(-zNear))/(zFar-zNear)
	// 0 0 0 1
	return scaleMatrix * translationMatrix;
}

mat4 geometry::getPerspectiveProjection(float left, float right, float bottom, float top,
										float zNear, float zFar)
{
	// P maps the perspective viewing volume (called frustum) to the orthographical viewing volume
	mat4 P{
		{zNear, 0, 0, 0},
		{0, zNear, 0, 0},
		{0, 0, zNear + zFar, zNear * zFar},
		{0, 0, -1, 0}
	};
	mat4 ortho = getOrthoProjection(left, right, bottom, top, zNear, zFar);
	return ortho * P;
}

mat4 geometry::getPerspectiveProjection(float fovy, float aspect, float zNear, float zFar)
{
	// fovy is the field of view in the y direction, in degrees
	// aspect is the ratio of the width to the height of the viewing volume
	// zNear and zFar are the distances to the near and far planes. They must be positive.
	float top = zNear * tan(radians(fovy) / 2);
	float right = top * aspect;
	return getPerspectiveProjection(-right, right, -top, top, zNear, zFar);
}

mat4 geometry::getViewPortTransform(int width, int height)
{
	mat4 viewPortTransform = mat4::Identity();
	viewPortTransform(0,0) = width / 2.0;
	viewPortTransform(1,1) = height / 2.0;

	viewPortTransform(0,3) = (width) / 2.0;
	viewPortTransform(1,3) = (height) / 2.0;
	
	viewPortTransform(2,2) = 1; // Keeping z values between [-1,1]
	
	return viewPortTransform;
}

mat4 geometry::makeRotationMatrix(const vec3& axis, float angle)
{
	return mat4 {
	{cos(angle) + axis.x() * axis.x() * (1 - cos(angle)), axis.x() * axis.y() * (1 - cos(angle)) - axis.z() * sin(angle), axis.x() * axis.z() * (1 - cos(angle)) + axis.y() * sin(angle), 0},
	{axis.y() * axis.x() * (1 - cos(angle)) + axis.z() * sin(angle), cos(angle) + axis.y() * axis.y() * (1 - cos(angle)), axis.y() * axis.z() * (1 - cos(angle)) - axis.x() * sin(angle), 0},
	{axis.z() * axis.x() * (1 - cos(angle)) - axis.y() * sin(angle), axis.z() * axis.y() * (1 - cos(angle)) + axis.x() * sin(angle), cos(angle) + axis.z() * axis.z() * (1 - cos(angle)), 0},
	{0, 0, 0, 1}
	};
}

mat4 geometry::makeRotationMatrix(float yaw, float pitch, float roll)
{
	mat4 yawMatrix{
		{cos(yaw), 0, sin(yaw), 0},
		{0, 1, 0, 0},
		{-sin(yaw), 0, cos(yaw), 0},
		{0, 0, 0, 1}
	};
	mat4 pitchMatrix{
		{1, 0, 0, 0},
		{0, cos(pitch), -sin(pitch), 0},
		{0, sin(pitch), cos(pitch), 0},
		{0, 0, 0, 1}
	};
	mat4 rollMatrix{
		{cos(roll), -sin(roll), 0, 0},
		{sin(roll), cos(roll), 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};
	return yawMatrix * pitchMatrix * rollMatrix;
}

mat4 geometry::makeScaleMatrix(const vec3& scale)
{
	return mat4{
		{scale.x(), 0, 0, 0},
		{0, scale.y(), 0, 0},
		{0, 0, scale.z(), 0},
		{0, 0, 0, 1}
	};
}

mat4 geometry::makeTranslationMatrix(vec3 translation)
{
	return mat4{
		{1, 0, 0, translation.x()},
		{0, 1, 0, translation.y()},
		{0, 0, 1, translation.z()},
		{0, 0, 0, 1}
	};
}


#include "stdafx.h"
#include "Geometry.h"
#include <cmath>

// We call this orthographic projection, but we don't really project, since we don't want to lose depth information.
// zNear and zFar are the distances to the near and far planes. They must be positive.
float Geometry::radians(float degrees) { return degrees * M_PI / 180; }

mat4 Geometry::getOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
	// translate center of orthographic viewing volume to origin (0,0,0)
	mat4 translationMatrix = mat4(
		vec4(1, 0, 0, -(right + left) / 2),
		vec4(0, 1, 0, -(top + bottom) / 2),
		vec4(0, 0, 1, (zFar + zNear) / 2),
		vec4(0, 0, 0, 1));
	// scale to make the orthographic viewing volume into a unit cube
	mat4 scaleMatrix = mat4(
		vec4(2 / (right - left), 0, 0, 0),
		vec4(0, 2 / (top - bottom), 0, 0),
		vec4(0, 0, -2 / (zFar - zNear), 0),
		vec4(0, 0, 0, 1));
	// The result looks like:
	// 2/(right-left) 0 0 -(right+left)/(right-left)
	// 0 2/(top-bottom) 0 -(top+bottom)/(top-bottom)
	// 0 0 -2/(zFar-zNear) -(zFar+zNear))/(zFar-zNear)
	// 0 0 0 1
	return scaleMatrix * translationMatrix;
}

mat4 Geometry::getPerspectiveProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
	// P maps the perspective viewing volume (called frustum) to the orthographical viewing volume
	// zNear and zFar are the distances to the near and far planes. They must be positive.
	mat4 P = mat4(
		vec4(zNear, 0, 0, 0),
		vec4(0, zNear, 0, 0),
		vec4(0, 0, zNear + zFar, zNear * zFar),
		vec4(0, 0, -1, 0));
	mat4 ortho = getOrthoProjection(left, right, bottom, top, zNear, zFar);
	return ortho * P;
	
}

mat4 Geometry::getPerspectiveProjection(float fovy, float aspect, float zNear, float zFar)
{
	// fovy is the field of view in the y direction, in degrees
	// aspect is the ratio of the width to the height of the viewing volume
	// zNear and zFar are the distances to the near and far planes. They must be positive.
	float top = zNear * tan(radians(fovy) / 2);
	float right = top * aspect;
	return getPerspectiveProjection(-right, right, -top, top, zNear, zFar);
}
mat4 Geometry::makeRotationMatrix(const vec3& axis, float angle)
{
	mat4 rotationMatrix = mat4(
		vec4(cos(angle) + axis.x * axis.x * (1 - cos(angle)), axis.x * axis.y * (1 - cos(angle)) - axis.z * sin(angle), axis.x * axis.z * (1 - cos(angle)) + axis.y * sin(angle), 0),
		vec4(axis.y * axis.x * (1 - cos(angle)) + axis.z * sin(angle), cos(angle) + axis.y * axis.y * (1 - cos(angle)), axis.y * axis.z * (1 - cos(angle)) - axis.x * sin(angle), 0),
		vec4(axis.z * axis.x * (1 - cos(angle)) - axis.y * sin(angle), axis.z * axis.y * (1 - cos(angle)) + axis.x * sin(angle), cos(angle) + axis.z * axis.z * (1 - cos(angle)), 0),
		vec4(0, 0, 0, 1));
	return rotationMatrix;
}

mat4 Geometry::makeRotationMatrix(float yaw, float pitch, float roll)
{
	mat4 yawMatrix = mat4(
		vec4(cos(yaw), 0, sin(yaw), 0),
		vec4(0, 1, 0, 0),
		vec4(-sin(yaw), 0, cos(yaw), 0),
		vec4(0, 0, 0, 1));
	mat4 pitchMatrix = mat4(
		vec4(1, 0, 0, 0),
		vec4(0, cos(pitch), -sin(pitch), 0),
		vec4(0, sin(pitch), cos(pitch), 0),
		vec4(0, 0, 0, 1));
	mat4 rollMatrix = mat4(
		vec4(cos(roll), -sin(roll), 0, 0),
		vec4(sin(roll), cos(roll), 0, 0),
		vec4(0, 0, 1, 0),
		vec4(0, 0, 0, 1));
	return yawMatrix * pitchMatrix * rollMatrix;
}

mat4 Geometry::makeScaleMatrix(const vec3& scale)
{
	return mat4(
		vec4(scale.x, 0, 0, 0),
		vec4(0, scale.y, 0, 0),
		vec4(0, 0, scale.z, 0),
		vec4(0, 0, 0, 1));
}

mat4 Geometry::makeTranslationMatrix(vec3 translation)
{
	return mat4(
		vec4(1, 0, 0, translation.x),
		vec4(0, 1, 0, translation.x),
		vec4(0, 0, 1, translation.x),
		vec4(0, 0, 0, 1));
}
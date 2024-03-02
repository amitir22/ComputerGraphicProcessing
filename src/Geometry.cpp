#include <cmath>
#include "Geometry.h"

// We call this orthographic projection, but we don't really project, since we don't want to lose depth information.
mat4 Geometry::getOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
	// translate center of orthographic viewing volume to origin (0,0,0)
	mat4 translationMatrix = mat4(
		vec4(1, 0, 0, -(right + left) / 2),
		vec4(0, 1, 0, -(top + bottom) / 2),
		vec4(0, 0, 1, -(-zFar + (-zNear)) / 2),
		vec4(0, 0, 0, 1));
	// scale to make the orthographic viewing volume into a unit cube
	mat4 scaleMatrix = mat4(
		vec4(2 / (right - left), 0, 0, 0),
		vec4(0, 2 / (top - bottom), 0, 0),
		vec4(0, 0, 2 / (zFar - zNear), 0),
		vec4(0, 0, 0, 1));
	// The result looks like:
	// 2/(right-left) 0 0 -(right+left)/(right-left)
	// 0 2/(top-bottom) 0 -(top+bottom)/(top-bottom)
	// 0 0 2/(zFar-zNear) -(-zFar+(-zNear))/(zFar-zNear)
	// 0 0 0 1
	return scaleMatrix * translationMatrix;
}

mat4 Geometry::getPerspectiveProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
	// P maps the perspective viewing volume (called frustum) to the orthographical viewing volume
	mat4 P = mat4(
		vec4(zNear, 0, 0, 0),
		vec4(0, zNear, 0, 0),
		vec4(0, 0, zNear + zFar, zNear * zFar),
		vec4(0, 0, -1, 0));
	mat4 ortho = getOrthoProjection(left, right, bottom, top, zNear, zFar);
	return ortho * P;
}

mat4 Geometry::getViewPortTransform(int width, int height)
{
	mat4 viewPortTransform = mat4(1.0);
	viewPortTransform[0][0] = width / 2.0;
	viewPortTransform[1][1] = height / 2.0;

	viewPortTransform[0][3] = (width) / 2.0;
	viewPortTransform[1][3] = (height) / 2.0;
	// Keeping z values between [-1,1]
	viewPortTransform[2][2] = 1;
	return viewPortTransform;
}



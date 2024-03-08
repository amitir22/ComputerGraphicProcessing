#include "Geometry.h"
#include <cmath> // for sin,cos

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif
float geometry::radians(float degrees) { return degrees * M_PI / 180; }

vec3 vec3fFromStream(std::istream& a_stream)
{
	float x, y, z;
	a_stream >> x >> std::ws >> y >> std::ws >> z;
	return vec3(x, y, z);
}

vec2 vec2fFromStream(std::istream& a_stream)
{
	float x, y;
	a_stream >> x >> std::ws >> y;
	return vec2(x, y);
}


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

void geometry::getTopAndRight(float fovy, float aspect, float zNear, float &top, float &right)
{
	// fovy is the field of view in the y direction, in degrees
	// aspect is the ratio of the width to the height of the viewing volume
	top = zNear * tan(radians(fovy) / 2);
	right = top * aspect;
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
	viewPortTransform(0, 0) = static_cast<float>(width) / 2.0f;
	viewPortTransform(1, 1) = static_cast<float>(height) / 2.0f;

	viewPortTransform(0, 3) = static_cast<float>(width) / 2.0f;
	viewPortTransform(1, 3) = static_cast<float>(height) / 2.0f;

	viewPortTransform(2,2) = 1; // Keeping z values between [-1,1]

	return viewPortTransform;
}

mat3 geometry::getNormalTransfrom(const mat4 &m)
{
    mat3 n;
    n(0,0) = m(1,1)*m(2,2) - m(1,2)*m(2,1);
    n(0,1) = m(1,2)*m(2,0) - m(1,0)*m(2,2);
    n(0,2) = m(1,0)*m(2,1) - m(1,1)*m(2,0);
    n(1,0) = m(0,2)*m(2,1) - m(0,1)*m(2,2);
    n(1,1) = m(0,0)*m(2,2) - m(0,2)*m(2,0);
    n(1,2) = m(0,1)*m(2,0) - m(0,0)*m(2,1);
    n(2,0) = m(0,1)*m(1,2) - m(0,2)*m(1,1);
    n(2,1) = m(0,2)*m(1,0) - m(0,0)*m(1,2);
    n(2,2) = m(0,0)*m(1,1) - m(0,1)*m(1,0);
    return n;
}

mat4 geometry::makeRotationMatrix(const vec3& axis, float angle)
{
	return mat4 {
		{static_cast<float>(cos(angle)) + axis.x() * axis.x() * (1 - static_cast<float>(cos(angle))), axis.x() * axis.y() * (1 - static_cast<float>(cos(angle))) - axis.z() * static_cast<float>(sin(angle)), axis.x() * axis.z() * (1 - static_cast<float>(cos(angle))) + axis.y() * static_cast<float>(sin(angle)), 0},
		{axis.y() * axis.x() * (1 - static_cast<float>(cos(angle))) + axis.z() * static_cast<float>(sin(angle)), static_cast<float>(cos(angle)) + axis.y() * axis.y() * (1 - static_cast<float>(cos(angle))), axis.y() * axis.z() * (1 - static_cast<float>(cos(angle))) - axis.x() * static_cast<float>(sin(angle)), 0},
		{axis.z() * axis.x() * (1 - static_cast<float>(cos(angle))) - axis.y() * static_cast<float>(sin(angle)), axis.z() * axis.y() * (1 - static_cast<float>(cos(angle))) + axis.x() * static_cast<float>(sin(angle)), static_cast<float>(cos(angle)) + axis.z() * axis.z() * (1 - static_cast<float>(cos(angle))), 0},
		{0, 0, 0, 1}
	};
}

mat4 geometry::makeRotationMatrix(float yaw, float pitch, float roll)
{
	mat4 yawMatrix{
		{static_cast<float>(cos(yaw)), 0, static_cast<float>(sin(yaw)), 0},
		{0, 1, 0, 0},
		{-static_cast<float>(sin(yaw)), 0, static_cast<float>(cos(yaw)), 0},
		{0, 0, 0, 1}
	};
	mat4 pitchMatrix{
		{1, 0, 0, 0},
		{0, static_cast<float>(cos(pitch)), -static_cast<float>(sin(pitch)), 0},
		{0, static_cast<float>(sin(pitch)), static_cast<float>(cos(pitch)), 0},
		{0, 0, 0, 1}
	};
	mat4 rollMatrix{
		{static_cast<float>(cos(roll)), -static_cast<float>(sin(roll)), 0, 0},
		{static_cast<float>(sin(roll)), static_cast<float>(cos(roll)), 0, 0},
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
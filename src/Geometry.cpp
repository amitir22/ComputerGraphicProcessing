#include "Geometry.h"
#include <cmath> // for sin,cos

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif
float geometry::Radians(float degrees) { return degrees * M_PI / 180; }

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
mat4 geometry::GetOrthoProjection(float left, float right, float bottom, float top,
								  float z_near, float z_far)
{
	// translate center of orthographic viewing volume to origin (0,0,0)
	mat4 translation_matrix{
		{1, 0, 0, -(right + left) / 2},
		{0, 1, 0, -(top + bottom) / 2},
		{0, 0, 1, -(-z_far + (-z_near)) / 2},
		{0, 0, 0, 1}
	};
	// scale to make the orthographic viewing volume into a unit cube
	mat4 scale_matrix{
		{2 / (right - left), 0, 0, 0},
		{0, 2 / (top - bottom), 0, 0},
		{0, 0, 2 / (z_far - z_near), 0},
		{0, 0, 0, 1}
	};
	// The result looks like:
	// 2/(right-left) 0 0 -(right+left)/(right-left)
	// 0 2/(top-bottom) 0 -(top+bottom)/(top-bottom)
	// 0 0 2/(z_far-z_near) -(-z_far+(-z_near))/(z_far-z_near)
	// 0 0 0 1
	return scale_matrix * translation_matrix;
}

mat4 geometry::GetPerspectiveProjection(float left, float right, float bottom, float top,
										float z_near, float z_far)
{
	// P maps the perspective viewing volume (called frustum) to the orthographical viewing volume
	mat4 P{
		{z_near, 0, 0, 0},
		{0, z_near, 0, 0},
		{0, 0, z_near + z_far, z_near * z_far},
		{0, 0, -1, 0}
	};
	mat4 ortho = GetOrthoProjection(left, right, bottom, top, z_near, z_far);
	return ortho * P;
}

void geometry::GetTopAndRight(float fovy, float aspect, float z_near, float &top, float &right)
{
	// fovy is the field of view in the y direction, in degrees
	// aspect is the ratio of the width to the height of the viewing volume
	top = z_near * tan(Radians(fovy) / 2);
	right = top * aspect;
}

mat4 geometry::GetPerspectiveProjection(float fovy, float aspect, float z_near, float z_far)
{
	// fovy is the field of view in the y direction, in degrees
	// aspect is the ratio of the width to the height of the viewing volume
	// z_near and z_far are the distances to the near and far planes. They must be positive.
	float top = z_near * tan(Radians(fovy) / 2);
	float right = top * aspect;
	return GetPerspectiveProjection(-right, right, -top, top, z_near, z_far);
}

mat4 geometry::GetViewportTransform(int width, int height)
{
	mat4 viewport_transform = mat4::Identity();
	viewport_transform(0, 0) = static_cast<float>(width) / 2.0f;
	viewport_transform(1, 1) = static_cast<float>(height) / 2.0f;

	viewport_transform(0, 3) = static_cast<float>(width) / 2.0f;
	viewport_transform(1, 3) = static_cast<float>(height) / 2.0f;

	viewport_transform(2,2) = 1; // Keeping z values between [-1,1]

	return viewport_transform;
}

mat3 geometry::GetNormalTransfrom(const mat4 &m)
{
    mat3 n = mat3::Identity();
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

//mat4 geometry::GetNormalTransfromFromModel(const mat4& model)
//{
//	mat4 n = mat4::Identity();
//	n(0, 0) = m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1);
//	n(0, 1) = m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2);
//	n(0, 2) = m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0);
//	n(1, 0) = m(0, 2) * m(2, 1) - m(0, 1) * m(2, 2);
//	n(1, 1) = m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0);
//	n(1, 2) = m(0, 1) * m(2, 0) - m(0, 0) * m(2, 1);
//	n(2, 0) = m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1);
//	n(2, 1) = m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2);
//	n(2, 2) = m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0);
//	return n;
//}

mat4 geometry::GetRotationMatrix(const vec3& axis, float angle)
{
	return mat4 {
		{static_cast<float>(cos(angle)) + axis.x() * axis.x() * (1 - static_cast<float>(cos(angle))), axis.x() * axis.y() * (1 - static_cast<float>(cos(angle))) - axis.z() * static_cast<float>(sin(angle)), axis.x() * axis.z() * (1 - static_cast<float>(cos(angle))) + axis.y() * static_cast<float>(sin(angle)), 0},
		{axis.y() * axis.x() * (1 - static_cast<float>(cos(angle))) + axis.z() * static_cast<float>(sin(angle)), static_cast<float>(cos(angle)) + axis.y() * axis.y() * (1 - static_cast<float>(cos(angle))), axis.y() * axis.z() * (1 - static_cast<float>(cos(angle))) - axis.x() * static_cast<float>(sin(angle)), 0},
		{axis.z() * axis.x() * (1 - static_cast<float>(cos(angle))) - axis.y() * static_cast<float>(sin(angle)), axis.z() * axis.y() * (1 - static_cast<float>(cos(angle))) + axis.x() * static_cast<float>(sin(angle)), static_cast<float>(cos(angle)) + axis.z() * axis.z() * (1 - static_cast<float>(cos(angle))), 0},
		{0, 0, 0, 1}
	};
}

mat4 geometry::GetRotationMatrix(float yaw, float pitch, float roll)
{
	mat4 yaw_matrix{
		{static_cast<float>(cos(yaw)), 0, static_cast<float>(sin(yaw)), 0},
		{0, 1, 0, 0},
		{-static_cast<float>(sin(yaw)), 0, static_cast<float>(cos(yaw)), 0},
		{0, 0, 0, 1}
	};
	mat4 pitch_matrix{
		{1, 0, 0, 0},
		{0, static_cast<float>(cos(pitch)), -static_cast<float>(sin(pitch)), 0},
		{0, static_cast<float>(sin(pitch)), static_cast<float>(cos(pitch)), 0},
		{0, 0, 0, 1}
	};
	mat4 roll_matrix{
		{static_cast<float>(cos(roll)), -static_cast<float>(sin(roll)), 0, 0},
		{static_cast<float>(sin(roll)), static_cast<float>(cos(roll)), 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};
	return yaw_matrix * pitch_matrix * roll_matrix;
}

mat4 geometry::GetScaleMatrix(const vec3& scale)
{
	return mat4{
		{scale.x(), 0, 0, 0},
		{0, scale.y(), 0, 0},
		{0, 0, scale.z(), 0},
		{0, 0, 0, 1}
	};
}

mat4 geometry::GetTranslationMatrix(vec3 translation)
{
	return mat4{
		{1, 0, 0, translation.x()},
		{0, 1, 0, translation.y()},
		{0, 0, 1, translation.z()},
		{0, 0, 0, 1}
	};
}
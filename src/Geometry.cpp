#include "Geometry.h"
#include <cmath> // for sin,cos

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif
float geometry::Radians(float degrees) { return degrees * M_PI / 180; }

vec3 geometry::RotateVector(const vec3& v, const vec3& axis, float angle)
{
	mat3 rotation_matrix = GetRotationMatrix(axis, angle).block<3, 3>(0, 0);
	return rotation_matrix * v;
}

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
	// z_near and z_far are the distances to the near and far planes. They must be positive.
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

mat4 geometry::GetOrthoProjection(float aspect, float fovy, float z_near, float z_far)
{
	float top, right;
	GetTopAndRight(fovy, aspect, z_near, top, right);
	return GetOrthoProjection(-right, right, -top, top, z_near, z_far);
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
	top = z_near * tan(Radians(fovy/2));
	right = top * aspect;
}

mat4 geometry::GetPerspectiveProjection(float fovy, float aspect, float z_near, float z_far)
{
	// fovy is the field of view in the y direction, in degrees
	// aspect is the ratio of the width to the height of the viewing volume
	// z_near and z_far are the distances to the near and far planes. They must be positive.
	float top, right; 
	GetTopAndRight(fovy, aspect, z_near, top, right);
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

geometry::Rectangle::Rectangle(const vec2& bottom_left, const vec2& top_right)
{
	bottom_left_ = bottom_left;
	top_right_ = top_right;
}

geometry::Rectangle::Rectangle()
{
	bottom_left_ = vec2();
	top_right_ = vec2();
}

geometry::Rectangle::Rectangle(float width, float height, const vec2& bottom_left)
{
	bottom_left_ = bottom_left;
	top_right_ = vec2(bottom_left.x() + width, bottom_left.y() + height);
}

geometry::Rectangle::Rectangle(float left_x, float right_x, float bottom_y, float top_y)
{
	bottom_left_ = vec2(left_x, bottom_y);
	top_right_ = vec2(right_x, top_y);
}

geometry::Rectangle::Rectangle(float width, float height)
{
	bottom_left_ = vec2();
	top_right_ = vec2(width, height);
}

geometry::Box::Box(const vec3& bottom_left_back, const vec3& top_right_front)
{
	bottom_left_back_ = bottom_left_back;
	top_right_front_ = top_right_front;
}

geometry::Box::Box()
{
	bottom_left_back_ = vec3();
	top_right_front_ = vec3();
}

geometry::Box::Box(float width, float height, float depth)
{
	bottom_left_back_ = vec3();
	top_right_front_ = vec3(width, height, depth);
}

geometry::Box::Box(float width, float height, float depth, const vec3& bottom_left_front)
{
	bottom_left_back_ = bottom_left_front;
	top_right_front_ = vec3(bottom_left_front.x() + width, bottom_left_front.y() + height, bottom_left_front.z() + depth);
}


float geometry::FastQuake3InverseSqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long*)&y;
	i = 0x5F3759DF - (i >> 1);
	y = *(float*)&i;
	y = y * (threehalfs - (x2 * y * y));

	return y;
}
#pragma once
#include "vec.h"
#include "mat.h"
namespace Geometry {
	float radians(float degrees);
	mat4 getPerspectiveProjection(float fovy, float aspect, float zNear, float zFar);
	mat4 makeRotationMatrix(const vec3& axis, float angle);
	mat4 makeRotationMatrix(float yaw, float pitch, float roll);
	mat4 makeScaleMatrix(const vec3& scale);
	mat4 makeTranslationMatrix(vec3 translation);
	mat4 getOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar);
	mat4 getPerspectiveProjection(float left, float right, float bottom, float top, float zNear, float zFar);
	
	struct Face {
		vec3 v[3];
		//vec3 normal;
	};
}
// Light.h
#pragma once
#include <vector>

#include "Material.h"

#define MAX_INTENSITY 100

class Light {
public:
	float intensity;
	Vector3f color;

	Light(float intensity, Vector3f color);
	float getLightIntensityAt(Vector3f fragment);
	Vector3f getLightDirectionAt(Vector3f fragment);
};

class PointLight : public Light {
public:
	Vector3f location; // TODO: maybe some other type?

	PointLight(float intensity, Vector3f color, Vector3f location);
	float getLightIntensityAt(Vector3f fragment);
	Vector3f getLightDirectionAt(Vector3f fragment);
};

class ParallelLight : public Light {
public:
	Vector3f direction;

	ParallelLight(float intensity, Vector3f color, Vector3f direction);
	float getLightIntensityAt(Vector3f fragment);
	Vector3f getLightDirectionAt(Vector3f fragment);
};

class AmbientLight : public Light {
public:
	AmbientLight(float intensity, Vector3f color);
	float getLightIntensityAt(Vector3f fragment);
	Vector3f getLightDirectionAt(Vector3f fragment);
};

Vector3f computeReflectionVector(Vector3f light_direction, Vector3f normalized_normal);
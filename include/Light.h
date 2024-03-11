// Light.h
#pragma once
#include <vector>

#include "Material.h"

#define MAX_LIGHT_INTENSITY (1.0)

// default light
static const vec3 DEFAULT_LIGHT_LOCATION = vec3(-0.5, 1, 2);
static const vec3 DEFAULT_LIGHT_COLOR = vec3(-0.5, 1, 2);
static const float DEFAULT_LIGHT_INTENSITY = 0.5;

class Light {
public:
	float intensity;
	Vector3f color;

	Light(float intensity, Vector3f color);
	virtual float getLightIntensityAt(Vector3f fragment);
	virtual Vector3f getLightDirectionAt(Vector3f fragment);
};

class PointLight : public Light {
public:
	Vector3f location; // TODO: maybe some other type?

	PointLight(float intensity = DEFAULT_LIGHT_INTENSITY,
				Vector3f color = DEFAULT_LIGHT_COLOR, 
				Vector3f location = DEFAULT_LIGHT_LOCATION);
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
	AmbientLight(float intensity=DEFAULT_MATERIAL_AMBIENT_FACTOR, Vector3f color=Vector3f(1,1,1));
	float getLightIntensityAt(Vector3f fragment);
	Vector3f getLightDirectionAt(Vector3f fragment);
};

Vector3f computeReflectionVector(Vector3f light_direction, Vector3f normalized_normal);
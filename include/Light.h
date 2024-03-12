// Light.h
#pragma once
#include <vector>

#include "Material.h"

#define MAX_LIGHT_INTENSITY (1.0)

// default light
static const vec3 DEFAULT_LIGHT_LOCATION = vec3(-0.5, 1, 2);
static const vec3 DEFAULT_LIGHT_COLOR = vec3(1, 1, 1);
static const float DEFAULT_LIGHT_INTENSITY = 0.5;

enum LightType {
	POINT_LIGHT,
	PARALLEL_LIGHT,
	AMBIENT_LIGHT
};

class Light {
public:
	float intensity;
	vec3 color;
	LightType type;

	Light(float intensity, vec3 color);
	virtual float getLightIntensityAt(vec3 fragment);
	virtual vec3 getLightDirectionAt(vec3 fragment);
};

class PointLight : public Light {
public:
	vec3 location; // TODO: maybe some other type?

	PointLight(float intensity = DEFAULT_LIGHT_INTENSITY,
				vec3 color = DEFAULT_LIGHT_COLOR, 
				vec3 location = DEFAULT_LIGHT_LOCATION);
	float getLightIntensityAt(vec3 fragment);
	vec3 getLightDirectionAt(vec3 fragment);
};

class ParallelLight : public Light {
public:
	vec3 direction;

	ParallelLight(float intensity, vec3 color, vec3 direction);
	float getLightIntensityAt(vec3 fragment);
	vec3 getLightDirectionAt(vec3 fragment);
};

class AmbientLight : public Light {
public:
	AmbientLight(float intensity=DEFAULT_MATERIAL_AMBIENT_FACTOR, vec3 color=vec3(1,1,1));
	float getLightIntensityAt(vec3 fragment);
	vec3 getLightDirectionAt(vec3 fragment);
};

vec3 computeReflectionVector(vec3 light_direction, vec3 normalized_normal);
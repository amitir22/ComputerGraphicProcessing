// Light.h
#pragma once
#include <vector>
#include "Constants.h"
#include "Material.h"
#include "MeshModel.h"

#define MAX_LIGHT_INTENSITY (3.0)

// default light
static const vec3 DEFAULT_LIGHT_LOCATION = vec3(-2, 1, -2);
static const vec3 DEFAULT_LIGHT_COLOR = vec3(1, 1, 1);
static const float DEFAULT_LIGHT_INTENSITY = 1.5;
static const float DEFAULT_BLOOM_INTENSITY_FACTOR = 1.0;


class Light {
public:
	float intensity;
	vec3 color;
	LightType type;

	Light(float intensity, vec3 color);
	vec3 GetColor() const { return this->color; }
	void SetColor(vec3 color) { this->color = color; }
	virtual float getLightIntensityAt(vec3 fragment);
	virtual vec3 getLightDirectionAt(vec3 fragment);
	LightType GetType() const { return type; }
	void SetIntensity(float intensity) { this->intensity = intensity; }
	float GetIntensity() { return this->intensity; }
};

class PointLight : public Light {
public:
	vec3 translation;
	MeshModel light_cube_model_;

	PointLight(float intensity = DEFAULT_LIGHT_INTENSITY,
				vec3 color = DEFAULT_LIGHT_COLOR, 
				vec3 translation = DEFAULT_LIGHT_LOCATION);
	float getLightIntensityAt(vec3 fragment);
	vec3 getLightDirectionAt(vec3 fragment);

	MeshModel* GetLightCubeModel() { return &light_cube_model_; }
	void SetTranslation(vec3 translation);
	vec3 GetTranslation() { return translation; }
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
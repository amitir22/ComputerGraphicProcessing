#include "Light.h"

Light::Light(float intensity, vec3 color)
{
	this->intensity = intensity;
	this->color = color;
}

float Light::getLightIntensityAt(vec3 fragment)
{
	assert(false, "Light is an abstract class");

	return 0.0f; // TODO: maybe assert error? should never be used.
}

vec3 Light::getLightDirectionAt(vec3 fragment)
{
	assert(false, "Light is an abstract class");
	return vec3();
}

PointLight::PointLight(float intensity, vec3 color,
	vec3 translation) : Light(intensity, color)
{
	this->translation = translation;
	type = POINT_LIGHT;
	this->light_cube_model_ = MeshModel(translation);
}

float PointLight::getLightIntensityAt(vec3 fragment)
{
	vec3 r = (fragment - this->translation);
	return geometry::FastQuake3InverseSqrt(this->intensity * r.size());
}

vec3 PointLight::getLightDirectionAt(vec3 fragment)
{
	return fragment - this->translation;
}

void PointLight::SetTranslation(vec3 translation)
{
	this->translation = translation;
	this->light_cube_model_.SetTranslation(translation);
}

ParallelLight::ParallelLight(float intensity, vec3 color,
	vec3 direction) : Light(intensity, color)
{
	this->direction = direction;
	type = PARALLEL_LIGHT;
}

float ParallelLight::getLightIntensityAt(vec3 fragment)
{
	// TODO: maybe replace with ray trace because there might be shadow?
	return this->intensity;
}

vec3 ParallelLight::getLightDirectionAt(vec3 fragment)
{
	return this->direction;
}

AmbientLight::AmbientLight(float intensity, vec3 color) : Light(intensity, color)
{
	type = AMBIENT_LIGHT;
}

float AmbientLight::getLightIntensityAt(vec3 fragment)
{
	return this->intensity;
}

vec3 AmbientLight::getLightDirectionAt(vec3 fragment)
{
	assert(false, "AmbientLight doesn't have a direction");

	return vec3();
}

vec3 computeReflectionVector(vec3 light_direction, vec3 normalized_normal)
{
	return light_direction - 2 * (light_direction.dot(normalized_normal) * normalized_normal);
}
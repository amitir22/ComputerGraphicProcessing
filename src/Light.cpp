#include "Light.h"

Light::Light(float intensity, Vector3f color)
{
	this->intensity = intensity;
	this->color = color;
}

float Light::getLightIntensityAt(Vector3f fragment)
{
	assert(false, "Light is an abstract class");

	return 0.0f; // TODO: maybe assert error? should never be used.
}

Vector3f Light::getLightDirectionAt(Vector3f fragment)
{
	assert(false, "Light is an abstract class");
	return Vector3f();
}

PointLight::PointLight(float intensity, Vector3f color,
	Vector3f location) : Light(intensity, color)
{
	this->location = location;
}

float PointLight::getLightIntensityAt(Vector3f fragment)
{
	Vector3f r = (fragment - this->location);
	return geometry::FastQuake3InverseSqrt(this->intensity * r.size());
}

Vector3f PointLight::getLightDirectionAt(Vector3f fragment)
{
	return fragment - this->location;
}

ParallelLight::ParallelLight(float intensity, Vector3f color,
	Vector3f direction) : Light(intensity, color)
{
	this->direction = direction;
}

float ParallelLight::getLightIntensityAt(Vector3f fragment)
{
	// TODO: maybe replace with ray trace because there might be shadow?
	return this->intensity;
}

Vector3f ParallelLight::getLightDirectionAt(Vector3f fragment)
{
	return this->direction;
}

AmbientLight::AmbientLight(float intensity, Vector3f color) : Light(intensity, color)
{
}

float AmbientLight::getLightIntensityAt(Vector3f fragment)
{
	return this->intensity;
}

Vector3f AmbientLight::getLightDirectionAt(Vector3f fragment)
{
	assert(false, "AmbientLight doesn't have a direction");

	return Vector3f();
}

Vector3f computeReflectionVector(Vector3f light_direction, Vector3f normalized_normal)
{
	return light_direction - 2 * (light_direction.dot(normalized_normal) * normalized_normal);
}
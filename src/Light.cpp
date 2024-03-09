#include "Light.h"

Light::Light(float intensity)
{
	this->intensity = intensity;
}

PointLight::PointLight(float intensity, Eigen::Vector3f location) : Light(intensity)
{
	this->location = location;
}

ParallelLight::ParallelLight(float intensity, Eigen::Vector3f direction) : Light(intensity)
{
	this->direction = direction;
}

AmbientLight::AmbientLight(float intensity) : Light(intensity)
{
}

#include "Material.h"

Material::Material(float smoothness, float k_ambient)
{
	this->k_ambient = k_ambient;
	this->setSmoothness(smoothness);
}

Vector3f Material::getColorFor(Eigen::Vector3f vertex)
{
	return Vector3f();
	// TODO: maybe change default? it should never be called thou... maybe replace with assert(false)?
}

float Material::getKAmbient()
{
	return this->k_ambient;
}

float Material::getKDiffuse()
{
	return this->k_diffuse;
}

float Material::getKSpecular()
{
	return this->k_specular;
}

void Material::setSmoothness(float smoothness)
{
	assert(0 <= smoothness && smoothness <= 1.0, "smoothness value range is [0.0, 1.0]");

	float diff = 1 - this->k_ambient;
	this->k_specular = smoothness * diff;
	this->k_diffuse = diff - this->k_specular;

	assert(this->k_ambient + this->k_diffuse + this->k_specular <= 1.0);
}


UniformMaterial::UniformMaterial()
{
}

UniformMaterial::UniformMaterial(Vector3f color)
{
	this->color = color;
}

Eigen::Vector3f UniformMaterial::getColorFor(Vector3f vertex)
{
	return this->color;
}

Eigen::Vector3f someLinearColoring(float x, float y, float z)
{
	Eigen::Vector3f color((x + y) - floor(x + y),
		(y + z) - floor(y + z),
		(z + x) - floor(z + x));

	return color;
}
VarMaterial::VarMaterial(colorFunction color_function)
{
	this->color_function = color_function;
}

Eigen::Vector3f VarMaterial::getColorFor(Vector3f vertex)
{
	return this->color_function(vertex.x(), vertex.y(), vertex.z());
}
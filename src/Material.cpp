#include "Material.h"

Material::Material()
{
}

Eigen::Vector3f Material::getColorFor(float x, float y, float z)
{
	return Eigen::Vector3f(); 
	// TODO: maybe change default? it should never be called thou... maybe replace with assert(false)?
}


UniformMaterial::UniformMaterial(Eigen::Vector3f color)
{
	this->color = color;
}

Eigen::Vector3f UniformMaterial::getColorFor(float x, float y, float z)
{
	return Eigen::Vector3f();
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

Eigen::Vector3f VarMaterial::getColorFor(float x, float y, float z)
{
	return Eigen::Vector3f();
}

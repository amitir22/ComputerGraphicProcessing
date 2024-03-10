// Material.h
#pragma once
#include <Geometry.h>

#define DEFAULT_MATERIAL_AMBIENT_FACTOR (0.1)

using Eigen::Vector3f;

typedef Vector3f(*colorFunction)(float, float, float);

class Material
{
	float k_diffuse; // Between 0 and 1.0
	float k_specular;
	float k_ambient;
public:

	Material(float smoothness = 0, float k_ambient = DEFAULT_MATERIAL_AMBIENT_FACTOR);
	virtual Vector3f getColorFor(Vector3f vertex);
	float getKAmbient();
	float getKDiffuse();
	float getKSpecular();
	void setSmoothness(float smoothness);
};

class UniformMaterial : public Material
{
	Vector3f color;

public:
	UniformMaterial();
	UniformMaterial(Vector3f color);
	Vector3f getColorFor(Vector3f vertex);
};

Vector3f someLinearColoring(float x, float y, float z);

class VarMaterial : public Material
{
	colorFunction color_function;

public:
	VarMaterial(colorFunction color_function = someLinearColoring);
	Vector3f getColorFor(Vector3f vertex);
};
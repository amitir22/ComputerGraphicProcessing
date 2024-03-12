// Material.h
#pragma once
#include <Geometry.h>

#define DEFAULT_MATERIAL_AMBIENT_FACTOR (0.1)
#define DEFAULT_MATERIAL_SMOOTHNESS (0.5)
#define DEFAULT_MATERIAL_SHININESS (32)

using Eigen::Vector3f;

typedef Vector3f(*colorFunction)(float, float, float);

class Material
{
	unsigned int shininess;
	float k_diffuse; // Between 0 and 1.0
	float k_specular;
	float k_ambient;
public:

	Material(unsigned int shininess = DEFAULT_MATERIAL_SHININESS,
			 float smoothness = DEFAULT_MATERIAL_SMOOTHNESS, 
			 float k_ambient = DEFAULT_MATERIAL_AMBIENT_FACTOR);
	virtual Vector3f getColorFor(Vector3f vertex);
	float getKAmbient();
	float getKDiffuse();
	float getKSpecular();
	unsigned int getShininess();
	void setShininess(unsigned int shininess);
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
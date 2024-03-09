#include <Geometry.h>

typedef Eigen::Vector3f(*colorFunction)(float, float, float);

class Material
{
public:
	Material();
	virtual Eigen::Vector3f getColorFor(float x, float y, float z);
};

class UniformMaterial : public Material
{
public:
	Eigen::Vector3f color;

	UniformMaterial(Eigen::Vector3f color);
	Eigen::Vector3f getColorFor(float x, float y, float z);
};

Eigen::Vector3f someLinearColoring(float x, float y, float z);

class VarMaterial : public Material
{
public:
	colorFunction color_function;

	VarMaterial(colorFunction color_function = someLinearColoring);
	Eigen::Vector3f getColorFor(float x, float y, float z);
};

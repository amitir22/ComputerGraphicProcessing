#include <vector>

#include "Geometry.h"

class Light {
public:
	float intensity;

	Light(float intensity);
	//virtual vector<fragment> getLightingForFragment(face, fragment[x, y, w0, w1, w2], material);
};

class PointLight : public Light {
public:
	Eigen::Vector3f location; // TODO: maybe some other type?

	PointLight(float intensity, Eigen::Vector3f location);
};

class ParallelLight : public Light {
public:
	Eigen::Vector3f direction;

	ParallelLight(float intensity, Eigen::Vector3f direction);
};

class AmbientLight : public Light {
public:
	AmbientLight(float intensity);
};
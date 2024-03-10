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
	vec3 location; // TODO: maybe some other type?

	PointLight(float intensity, vec3 location);
};

class ParallelLight : public Light {
public:
	vec3 direction;

	ParallelLight(float intensity, vec3 direction);
};

class AmbientLight : public Light {
public:
	AmbientLight(float intensity);
};
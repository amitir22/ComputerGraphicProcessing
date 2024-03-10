#include "Fragment.h"

Fragment::Fragment()
{
	this->barycentric_w0 = 0;
	this->barycentric_w1 = 0;
	this->barycentric_w2 = 0;
}

vec3 Fragment::ComputeColorFlat(std::vector<Light*> lights)
{
	vec3 color = vec3(1, 0, 0);
	vec3 color_diffuse;
	vec3 color_specular;
	float intensity_sum = 0;

	for (auto &light : lights)
	{
		float current_intensity = light->getLightIntensityAt(this->v_center);
		vec3 c1 = this->material.getColorFor(this->v1);
		vec3 c2 = this->material.getColorFor(this->v2);
		vec3 c3 = this->material.getColorFor(this->v3);
		vec3 l = light->getLightDirectionAt(this->v_center);
		vec3 n = this->face_normal.normalized();

		if (intensity_sum < MAX_INTENSITY)
		{
			intensity_sum += current_intensity;
		}

		if (intensity_sum < MAX_INTENSITY)
		{
			intensity_sum = MAX_INTENSITY;
		}
	}

	return color;
}

vec3 Fragment::ComputeColorGouraud(std::vector<Light*> lights)
{
	vec3 color = vec3(0, 1, 0);

	for (auto& light : lights)
	{
		// TODO:
	}

	return color;
}

vec3 Fragment::ComputeColorPhong(std::vector<Light*> lights)
{
	vec3 color = vec3(0, 0, 1);

	for (auto& light : lights)
	{
		// TODO:
	}

	return color;
}
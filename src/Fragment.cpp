#include "Fragment.h"

Fragment::Fragment()
{
	this->barycentric_w0 = 0;
	this->barycentric_w1 = 0;
	this->barycentric_w2 = 0;
}

Vector3f Fragment::computeColorFlat(std::vector<Light> lights)
{
	Vector3f color;
	Vector3f color_diffuse;
	Vector3f color_specular;
	float intensity_sum = 0;

	for (auto &light : lights)
	{
		float current_intensity = light.getLightIntensityAt(Vector3f(this->v_center));
		Vector3f c1 = this->material.getColorFor(this->v1);
		Vector3f c2 = this->material.getColorFor(this->v2);
		Vector3f c3 = this->material.getColorFor(this->v3);
		Vector3f l = light.getLightDirectionAt(this->v_center);
		Vector3f n = this->face_normal.normalized();

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

Vector3f Fragment::computeColorGourard(std::vector<Light> lights)
{
	Vector3f color;

	for (auto& light : lights)
	{
		// TODO:
	}

	return color;
}

Vector3f Fragment::computeColorPhong(std::vector<Light> lights)
{
	Vector3f color;

	for (auto& light : lights)
	{
		// TODO:
	}

	return color;
}

#include "Fragment.h"

Fragment::Fragment()
{
	this->barycentric_w0 = 0;
	this->barycentric_w1 = 0;
	this->barycentric_w2 = 0;
}

vec3 Fragment::ComputeColorFlat(std::vector<Light*> lights, Light* ambient_light, Camera* camera)
{
	vec3 color = vec3(1, 0, 0);
	vec3 color_diffuse;
	vec3 color_specular;
	vec3 color_ambient;
	float intensity_sum = 0;

	vec3 c1 = this->material.getColorFor(this->v1);
	vec3 c2 = this->material.getColorFor(this->v2);
	vec3 c3 = this->material.getColorFor(this->v3);
	vec3 n = this->face_normal.normalized();
	vec3 v = camera->eye_ - this->v_center;
	float k_a = this->material.getKAmbient();
	float k_d = this->material.getKDiffuse();
	float k_s = this->material.getKSpecular();

	color_ambient = ambient_light->intensity * ambient_light->color;
		
	for (auto &light : lights)
	{
		vec3 light_direction = light->getLightDirectionAt(this->v_center);
		vec3 l = -light_direction; // vector from: this->v_center to: light source
		vec3 r = computeReflectionVector(light_direction, n);
		float current_intensity = light->getLightIntensityAt(this->v_center);
		float diffuse_intensity = fmax(0, n.dot(l));
		float specular_intensity = pow(fmax(v.dot(r), 0.0), this->material.getShininess());

		color_diffuse = diffuse_intensity * light->color;
		color_specular = specular_intensity * light->color;
		if (intensity_sum < MAX_LIGHT_INTENSITY)
		{
			intensity_sum += current_intensity;
		}
		if (intensity_sum < MAX_LIGHT_INTENSITY)
		{
			intensity_sum = MAX_LIGHT_INTENSITY;
		}

		color = k_a * color_ambient + k_d * color_diffuse + k_s * color_specular;

		// clipping color values
		if (color.x() > 1)
			color[0] = 1;
		if (color.y() > 1)
			color[1] = 1;
		if (color.z() > 1)
			color[2] = 1;
	}

	return color;
}

vec3 Fragment::ComputeColorGouraud(std::vector<Light*> lights, Light* ambient_light, Camera* camera)
{
	vec3 color = vec3(0, 1, 0);

	for (auto& light : lights)
	{
		// TODO:
	}

	return color;
}

vec3 Fragment::ComputeColorPhong(std::vector<Light*> lights, Light* ambient_light, Camera* camera)
{
	vec3 color = vec3(0, 0, 1);

	for (auto& light : lights)
	{
		// TODO:
	}

	return color;
}
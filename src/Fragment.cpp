#include "Fragment.h"

Fragment::Fragment()
{
	this->barycentric_w0 = 0;
	this->barycentric_w1 = 0;
	this->barycentric_w2 = 0;
}

vec3 Fragment::ComputeColorFlat(std::vector<Light*> lights, Light* ambient_light, Camera* camera)
{
	vec3 color = vec3::Zero();
	vec3 color_diffuse;
	vec3 color_specular;
	vec3 color_ambient;

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

		color += current_intensity * (k_a * color_ambient + k_d * color_diffuse + k_s * color_specular);

		// clipping color values
		if (color[0] > 1)
			color[0] = 1;
		if (color[1] > 1)
			color[1] = 1;
		if (color[2] > 1)
			color[2] = 1;
	}

	return color;
}

vec3 Fragment::ComputeColorGouraud(std::vector<Light*> lights, Light* ambient_light, Camera* camera)
{
	vec3 color;
	vec3 color_diffuse;
	vec3 color_specular;
	vec3 color_ambient;
	float intensity_sum = 0;

	vec3 c1 = vec3::Zero();
	vec3 c2 = vec3::Zero();
	vec3 c3 = vec3::Zero();
	vec3 n = this->face_normal.normalized();
	vec3 v = camera->eye_ - this->v_center;
	float k_a = this->material.getKAmbient();
	float k_d = this->material.getKDiffuse();
	float k_s = this->material.getKSpecular();

	color_ambient = ambient_light->intensity * ambient_light->color;

	for (auto& light : lights)
	{
		vec3 light_direction1 = light->getLightDirectionAt(this->v1);
		vec3 l1 = -light_direction1; // vector from: this->v_center to: light source
		vec3 r1 = computeReflectionVector(light_direction1, n);
		float current_intensity1 = light->getLightIntensityAt(this->v1);
		float diffuse_intensity1 = fmax(0, n.dot(l1));
		float specular_intensity1 = pow(fmax(v.dot(r1), 0.0), this->material.getShininess());

		color_diffuse = diffuse_intensity1 * light->color;
		color_specular = specular_intensity1 * light->color;

		c1 += current_intensity1 * (k_a * color_ambient + k_d * color_diffuse + k_s * color_specular);

		vec3 light_direction2 = light->getLightDirectionAt(this->v2);
		vec3 l2 = -light_direction2; // vector from: this->v_center to: light source
		vec3 r2 = computeReflectionVector(light_direction2, n);
		float current_intensity2 = light->getLightIntensityAt(this->v2);
		float diffuse_intensity2 = fmax(0, n.dot(l2));
		float specular_intensity2 = pow(fmax(v.dot(r2), 0.0), this->material.getShininess());

		color_diffuse = diffuse_intensity2 * light->color;
		color_specular = specular_intensity2 * light->color;

		c2 += current_intensity2 * (k_a * color_ambient + k_d * color_diffuse + k_s * color_specular);

		vec3 light_direction3 = light->getLightDirectionAt(this->v3);
		vec3 l3 = -light_direction3; // vector from: this->v_center to: light source
		vec3 r3 = computeReflectionVector(light_direction3, n);
		float current_intensity3 = light->getLightIntensityAt(this->v3);
		float diffuse_intensity3 = fmax(0, n.dot(l3));
		float specular_intensity3 = pow(fmax(v.dot(r3), 0.0), this->material.getShininess());

		color_diffuse = diffuse_intensity3 * light->color;
		color_specular = specular_intensity3 * light->color;

		c3 += current_intensity3 * (k_a * color_ambient + k_d * color_diffuse + k_s * color_specular);
	}

	color = this->barycentric_w0 * c1 + this->barycentric_w1 * c2 + this->barycentric_w2 * c3;

	// clipping color values
	if (color[0] > 1)
		color[0] = 1;
	if (color[1] > 1)
		color[1] = 1;
	if (color[2] > 1)
		color[2] = 1;

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
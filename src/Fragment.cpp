#include "Fragment.h"

Fragment::Fragment()
{
	this->barycentric_w0 = 0;
	this->barycentric_w1 = 0;
	this->barycentric_w2 = 0;
}

vec3 computeColorContribution(Light *light, Camera* cam, vec3 location, vec3 n, 
							  vec3 color_ambient, Material material, 
							  float k_a, float k_d, float k_s)
{
	float current_intensity = light->getLightIntensityAt(location);

	// ambient
	vec3 ambient = k_a * color_ambient;

	// diffuse
	vec3 light_direction = light->getLightDirectionAt(location).normalized();

	vec3 direction_to_light = -light_direction; // vector from: location to: light source
	//vec3 l = direction_to_light; // different notation, exactly the same
	float cos_theta = fmax(direction_to_light.dot(n), 0.0);
	vec3 diffuse = (current_intensity * k_d * cos_theta * light->color).cwiseProduct(material.getColorFor(location)); // TODO: maybe multiply k_d?

	// specular
	vec3 v = (cam->eye_ - location).normalized();
	vec3 r = computeReflectionVector(light_direction, n);
	float spec = powf(fmax(v.dot(r), 0.0), material.getShininess());
	vec3 specular = current_intensity * k_s * spec * light->color;

	return ambient + diffuse + specular;
}

vec3 Fragment::ComputeColorFlat(std::vector<Light*> lights, Light* ambient_light, 
								Camera* camera)
{
	vec3 color = vec3::Zero();
	vec3 color_ambient;
	vec3 current_color_contribution;

	vec3 n = this->face_normal.normalized();
	float k_a = this->material.getKAmbient();
	float k_d = this->material.getKDiffuse();
	float k_s = this->material.getKSpecular();

	color_ambient = ambient_light->intensity * ambient_light->color;
		
	for (auto &light : lights)
	{
		color += computeColorContribution(light, camera, this->v_center, n, color_ambient, this->material, k_a, k_d, k_s);

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
	vec3 color = vec3::Zero();
	vec3 color_ambient;
	float intensity_sum = 0;

	vec3 c1 = vec3::Zero();
	vec3 c2 = vec3::Zero();
	vec3 c3 = vec3::Zero();
	float k_a = this->material.getKAmbient();
	float k_d = this->material.getKDiffuse();
	float k_s = this->material.getKSpecular();

	color_ambient = ambient_light->intensity * ambient_light->color;

	for (auto& light : lights)
	{
		c1 += computeColorContribution(light, camera, this->v1, this->v1_n.normalized(),
			color_ambient, this->material, k_a, k_d, k_s);

		c2 += computeColorContribution(light, camera, this->v2, this->v2_n.normalized(),
			color_ambient, this->material, k_a, k_d, k_s);

		c3 += computeColorContribution(light, camera, this->v3, this->v3_n.normalized(),
			color_ambient, this->material, k_a, k_d, k_s);

		// clipping color values
		if (c1[0] > 1)
			c1[0] = 1;
		if (c1[1] > 1)
			c1[1] = 1;
		if (c1[2] > 1)
			c1[2] = 1;
		if (c2[0] > 1)
			c2[0] = 1;
		if (c2[1] > 1)
			c2[1] = 1;
		if (c2[2] > 1)
			c2[2] = 1;
		if (c3[0] > 1)
			c3[0] = 1;
		if (c3[1] > 1)
			c3[1] = 1;
		if (c3[2] > 1)
			c3[2] = 1;
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
	vec3 color = vec3::Zero();
	vec3 color_ambient = ambient_light->intensity * ambient_light->color;

	vec3 n = (this->barycentric_w0 * this->v1_n + this->barycentric_w1 * this->v2_n + 
			 this->barycentric_w2 * this->v3_n).normalized();
	vec3 location = this->barycentric_w0 * this->v1 + this->barycentric_w1 * this->v2 + 
			 this->barycentric_w2 * this->v3;
	float k_a = this->material.getKAmbient();
	float k_d = this->material.getKDiffuse();
	float k_s = this->material.getKSpecular();

	for (auto& light : lights)
	{
		color += computeColorContribution(light, camera, location, n, color_ambient, this->material,
										  k_a, k_d, k_s);

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
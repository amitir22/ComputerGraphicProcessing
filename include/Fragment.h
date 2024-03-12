// Fragment.h
#pragma once
#include <cmath>
#include "Light.h"
#include "Camera.h"

class Fragment {
public:
	Material* material;
	vec3 v1;
	vec3 v2;
	vec3 v3;
	vec3 v1_n;
	vec3 v2_n;
	vec3 v3_n;
	vec3 face_normal;
	float barycentric_w0;
	float barycentric_w1;
	float barycentric_w2;

	Fragment();

	Fragment(Material* material, 
		const vec3& v1, 
		const vec3& v2, 
		const vec3& v3, 
		const vec3& v1_n, 
		const vec3& v2_n, 
		const vec3& v3_n,
		const vec3& face_normal,
		float barycentric_w0, 
		float barycentric_w1, 
		float barycentric_w2)
		: material(material), v1(v1), v2(v2), v3(v3), v1_n(v1_n), v2_n(v2_n), 
			v3_n(v3_n), face_normal(face_normal), barycentric_w0(barycentric_w0), 
			barycentric_w1(barycentric_w1), barycentric_w2(barycentric_w2)
	{}

	vec3 ComputeColorFlat(std::vector<Light*>, Light* ambient_light, Camera *camera);
	vec3 ComputeColorGouraud(std::vector<Light*>, Light* ambient_light, Camera *camera);
	vec3 ComputeColorPhong(std::vector<Light*>, Light* ambient_light, Camera *camera);
};

vec3 computeColorContribution(Light* light, Camera* cam, vec3 location, 
	vec3 n,	vec3 color_ambient, Material* material,
	float k_a, float k_d, float k_s);
// Fragment.h
#pragma once
#include "Light.h"

class Fragment {
public:
	Material material;
	Vector3f v1;
	Vector3f v2;
	Vector3f v3;
	Vector3f v_center;
	Vector3f v1_n;
	Vector3f v2_n;
	Vector3f v3_n;
	float barycentric_w0;
	float barycentric_w1;
	float barycentric_w2;

	Fragment();

	Fragment(const Material& material, 
		const Vector3f& v1, 
		const Vector3f& v2, 
		const Vector3f& v3, 
		const Vector3f& v1_n, 
		const Vector3f& v2_n, 
		const Vector3f& v3_n, 
		float barycentric_w0, 
		float barycentric_w1, 
		float barycentric_w2)
		: material(material), v1(v1), v2(v2), v3(v3), v1_n(v1_n), v2_n(v2_n), v3_n(v3_n), barycentric_w0(barycentric_w0), barycentric_w1(barycentric_w1), barycentric_w2(barycentric_w2)
	{
		this->v_center = (v1 + v2 + v3) / 3;
	}

	Vector3f computeColorFlat(std::vector<Light>);
	Vector3f computeColorGourard(std::vector<Light>);
	Vector3f computeColorPhong(std::vector<Light>);
};

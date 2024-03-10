#include "Fragment.h"

Fragment::Fragment()
{
	this->barycentric_w0 = 0;
	this->barycentric_w1 = 0;
	this->barycentric_w2 = 0;
}

Vector3f Fragment::computeColorFlat(std::vector<Light>)
{
	return Vector3f();
}

Vector3f Fragment::computeColorGourard(std::vector<Light>)
{
	return Vector3f();
}

Vector3f Fragment::computeColorPhong(std::vector<Light>)
{
	return Vector3f();
}

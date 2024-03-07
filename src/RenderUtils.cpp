#include "RenderUtils.h"

#include <cmath>


float EdgeFunction(const Eigen::Vector3f& a, const Eigen::Vector3f& b, const Eigen::Vector3f& c) {
	return (c.x() - a.x()) * (b.y() - a.y()) - (c.y() - a.y()) * (b.x() - a.x());
}

int GetIntMin(float a, float b, float c) { return static_cast<int>(std::floor(std::min(a, std::min(b, c))));}
int GetIntMax(float a, float b, float c) {return static_cast<int>(std::ceil(std::max(a, std::max(b, c))));}
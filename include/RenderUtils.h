#pragma once
#include <Eigen/Dense>
#include <cmath>

inline float EdgeFunction(const Eigen::Vector3f& a, const Eigen::Vector3f& b, const Eigen::Vector3f& c) {
	return (c.x() - a.x()) * (b.y() - a.y()) - (c.y() - a.y()) * (b.x() - a.x());
}


inline int GetIntMin(float a, float b, float c) { return static_cast<int>(std::floor(std::min(a, std::min(b, c)))); }
inline int GetIntMax(float a, float b, float c) { return static_cast<int>(std::ceil(std::max(a, std::max(b, c)))); }

inline bool IsInBoundingBox(int x0, int x1, int y0, int y1, int x, int y) {
	return (x >= x0 && x <= x1 && y >= y0 && y <= y1);
}

inline void GetBoundingBox(vec3 v0, vec3 v1, vec3 v2, int& min_x, int& max_x, int& min_y, int& max_y) {
	min_x = GetIntMin(v0.x(), v1.x(), v2.x());
	max_x = GetIntMax(v0.x(), v1.x(), v2.x());
	min_y = GetIntMin(v0.y(), v1.y(), v2.y());
	max_y = GetIntMax(v0.y(), v1.y(), v2.y());
}
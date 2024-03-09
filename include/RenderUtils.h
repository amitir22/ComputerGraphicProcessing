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

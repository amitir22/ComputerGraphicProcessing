// Clipper.h
#pragma
#include "Geometry.h"
using namespace geometry;
class FrustumClipper { // Clipper in view space, camera facing to the negative z-axis
public:
	// HalfPlane* half_planes_;
	HalfPlane bottom_plane_;
	HalfPlane top_plane_;
	HalfPlane left_plane_;
	HalfPlane right_plane_;
	HalfPlane near_plane_;
	HalfPlane far_plane_;
	// n, f, r, l, t, b are unit vectors
	FrustumClipper(vec3 n, vec3 f, vec3 r, vec3 l, vec3 t, vec3 b, float z_near, float z_far);

	bool IsInside(const vec3& point) const {
		return near_plane_.IsInside(point) && far_plane_.IsInside(point) &&
			left_plane_.IsInside(point) && right_plane_.IsInside(point) &&
			bottom_plane_.IsInside(point) && top_plane_.IsInside(point);
	}

	float GetMinDistance(const vec3& point);

	bool IsSphereInside(const vec3& center, float radius);
};
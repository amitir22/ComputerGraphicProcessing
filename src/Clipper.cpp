#include "Clipper.h"

FrustumClipper::FrustumClipper(vec3 n, vec3 f, vec3 r, vec3 l, vec3 t, vec3 b, float z_near, float z_far)
{
	// z_near, z_far are positive
	near_plane_ = HalfPlane(n, -z_near);
	far_plane_ = HalfPlane(f, z_far);
	left_plane_ = HalfPlane(l, 0);
	right_plane_ = HalfPlane(r, 0);
	bottom_plane_ = HalfPlane(b, 0);
	top_plane_ = HalfPlane(t, 0);
}

float FrustumClipper::GetMinDistance(const vec3& point)
{
	float min_distance = near_plane_.Distance(point);
	min_distance = std::min(min_distance, far_plane_.Distance(point));
	min_distance = std::min(min_distance, left_plane_.Distance(point));
	min_distance = std::min(min_distance, right_plane_.Distance(point));
	min_distance = std::min(min_distance, bottom_plane_.Distance(point));
	min_distance = std::min(min_distance, top_plane_.Distance(point));
	return min_distance;
}

bool FrustumClipper::IsSphereInside(const vec3& center, float radius)
{
	return this->IsInside(center) && (radius < this->GetMinDistance(center) );
}


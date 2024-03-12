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


FrustumClipper::FrustumClipper(Camera* camera)
{
	float z_near = camera->GetZNear();
	float z_far = camera->GetZFar();
	float aspect = camera->GetAspect();
	float fovy = geometry::Radians(camera->GetFovy());
	vec3 n = vec3(0, 0, -1);
	vec3 f = vec3(0, 0, 1);
	// calculate the normal to the top half-plane, using fovy 
	vec3 t = vec3(0, -cos(fovy/2), -sin(fovy/2));
	vec3 b = vec3(0, cos(fovy / 2), -sin(fovy / 2));
	vec3 r = vec3(-1, 0, -tan(fovy/2)*aspect).normalized();
	vec3 l = vec3(1, 0, -tan(fovy/2)*aspect).normalized();
	// call frustum clipper constructor with vec3s
	FrustumClipper(n, f, r, l, t, b, z_near, z_far);
}

void FrustumClipper::SetCamera(Camera* camera)
{
	float z_near = camera->GetZNear();
	float z_far = camera->GetZFar();
	float aspect = camera->GetAspect();
	float fovy = geometry::Radians(camera->GetFovy());
	vec3 n = vec3(0, 0, -1);
	vec3 f = vec3(0, 0, 1);
	// calculate the normal to the top half-plane, using fovy 
	vec3 t = vec3(0, -cos(fovy / 2), -sin(fovy / 2));
	vec3 b = vec3(0, cos(fovy / 2), -sin(fovy / 2));
	vec3 r = vec3(-1, 0, -tan(fovy / 2) * aspect).normalized();
	vec3 l = vec3(1, 0, -tan(fovy / 2) * aspect).normalized();
	near_plane_ = HalfPlane(n, -z_near);
	far_plane_ = HalfPlane(f, z_far);
	left_plane_ = HalfPlane(l, 0);
	right_plane_ = HalfPlane(r, 0);
	bottom_plane_ = HalfPlane(b, 0);
	top_plane_ = HalfPlane(t, 0);
}



bool FrustumClipper::IsInside(const vec3& point)
{
	if (!current_point.isApprox(point)) {
		current_point = point;
		std::cout << "current_point: " << current_point.transpose() << std::endl;
		float near_dist = near_plane_.Distance(point);
		float far_dist = far_plane_.Distance(point);
		float left_dist = left_plane_.Distance(point);
		float right_dist = right_plane_.Distance(point);
		float bottom_dist = bottom_plane_.Distance(point);
		float top_dist = top_plane_.Distance(point);
		/*std::cout << "near_dist: " << near_dist << " far_dist: " << far_dist << " left_dist: " << left_dist << " right_dist: " << right_dist << " bottom_dist: " << bottom_dist << " top_dist: " << top_dist << std::endl;*/
	}
	
	return near_plane_.IsInside(point) && far_plane_.IsInside(point) &&
		left_plane_.IsInside(point) && right_plane_.IsInside(point) &&
		bottom_plane_.IsInside(point) && top_plane_.IsInside(point);
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

bool FrustumClipper::IsSphereOutside(const vec3& center, float radius)
{
	return !this->IsInside(center) && (this->GetMinDistance(center) < -radius);
}


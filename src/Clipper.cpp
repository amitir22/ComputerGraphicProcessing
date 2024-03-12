#include "Clipper.h"


std::vector<HalfPlane*> FrustumClipper::GetPlanes() {
	std::vector<HalfPlane*> planes;
	planes.push_back(&near_plane_);
	planes.push_back(&far_plane_);
	planes.push_back(&left_plane_);
	planes.push_back(&right_plane_);
	planes.push_back(&bottom_plane_);
	planes.push_back(&top_plane_);
	return planes;
}

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

bool FrustumClipper::IsSphereCompletelyInside(const vec3& center, float radius)
{
	return this->IsInside(center) && (radius < this->GetMinDistance(center) );
}

bool FrustumClipper::IsSphereCompletelyOutside(const vec3& center, float radius)
{
	return !this->IsInside(center) && (this->GetMinDistance(center) < -radius);
}

float Intersection(const vec3& A, const vec3& B, const HalfPlane& plane) {
	vec3 N = plane.normal_;
	float D = plane.d_;
	float numerator = -D - N.dot(A);
	float denominator = N.dot(B - A);
	return numerator / denominator; // t
}

std::vector<vec3> FrustumClipper::ClipTriangleWithPlane(vec3& v0, vec3& v1, vec3& v2, const HalfPlane &plane)
{
    std::vector<vec3> triangles_vertices_camera;
	float d0 = plane.Distance(v0);
	float d1 = plane.Distance(v1);
	float d2 = plane.Distance(v2);
	// all inside
	if (d0 > 0 && d1 > 0 && d2 > 0) {
		triangles_vertices_camera.push_back(v0);
		triangles_vertices_camera.push_back(v1);
		triangles_vertices_camera.push_back(v2);
		return triangles_vertices_camera;
	}
	// all outside
	if (d0 < 0 && d1 < 0 && d2 < 0) {
		return triangles_vertices_camera;
	}
	// if only one of {d0, d1, d2} is positive
	if (d0 > 0 && d1 < 0 && d2 < 0) {
		// v0 is inside
		float tAB = Intersection(v0, v1, plane);
		vec3 B = v0 + tAB * (v1 - v0);
		float tAC = Intersection(v0, v2, plane);
		vec3 C = v0 + tAC * (v2 - v0);
		triangles_vertices_camera.push_back(v0);
		triangles_vertices_camera.push_back(B);
		triangles_vertices_camera.push_back(C);
		return triangles_vertices_camera;

	}
	else if (d1 > 0 && d0 < 0 && d2 < 0) {
		// v1 is inside
		float tBA = Intersection(v1, v0, plane);
		vec3 A = v1 + tBA * (v0 - v1);
		float tBC = Intersection(v1, v2, plane);
		vec3 C = v1 + tBC * (v2 - v1);
		triangles_vertices_camera.push_back(v1);
		triangles_vertices_camera.push_back(A);
		triangles_vertices_camera.push_back(C);
		return triangles_vertices_camera;
	}
	else if (d2 > 0 && d0 < 0 && d1 < 0) {
		// v2 is inside
		float tCA = Intersection(v2, v0, plane);
		vec3 A = v2 + tCA * (v0 - v2);
		float tCB = Intersection(v2, v1, plane);
		vec3 B = v2 + tCB * (v1 - v2);
		triangles_vertices_camera.push_back(v2);
		triangles_vertices_camera.push_back(A);
		triangles_vertices_camera.push_back(B);
		return triangles_vertices_camera;
	}

	// if only one of {d0,d1,d2} is negative
	if (d0 > 0 && d1 > 0 && d2 < 0) {
		// let v2 be the vertex with a negative distance
		// compute A = Intersection(v0v2, plane)
		float tAC = Intersection(v0, v2, plane);
		vec3 A = v0 + tAC * (v2 - v0);
        // compute B = Intersection(v1v2, plane)
		float tBC = Intersection(v1, v2, plane);
		vec3 B = v1 + tBC * (v2 - v1);
        // return [Triangle(v0, v1, A), Triangle(A, v1, B)]
		triangles_vertices_camera.push_back(v0);
		triangles_vertices_camera.push_back(v1);
		triangles_vertices_camera.push_back(A);
		triangles_vertices_camera.push_back(A);
		triangles_vertices_camera.push_back(v1);
		triangles_vertices_camera.push_back(B);
		return triangles_vertices_camera;
	}
	else if (d0 > 0 && d2 > 0 && d1 < 0) {
		// let v1 be the vertex with a negative distance
		// compute A = Intersection(v0v1, plane)
		float tAB = Intersection(v0, v1, plane);
		vec3 A = v0 + tAB * (v1 - v0);
		// compute B = Intersection(v2v1, plane)
		float tCB = Intersection(v2, v1, plane);
		vec3 C = v2 + tCB * (v1 - v2);
		// return [Triangle(v0, A, v2), Triangle(A, v2, C)]
		triangles_vertices_camera.push_back(v0);
		triangles_vertices_camera.push_back(A);
		triangles_vertices_camera.push_back(v2);
		triangles_vertices_camera.push_back(A);
		triangles_vertices_camera.push_back(v2);
		triangles_vertices_camera.push_back(C);
		return triangles_vertices_camera;
	}
	else if (d1 > 0 && d2 > 0 && d0 < 0) {
		// let v0 be the vertex with a negative distance
		// compute A = Intersection(v1v0, plane)
		float tBA = Intersection(v1, v0, plane);
		vec3 B = v1 + tBA * (v0 - v1);
		// compute B = Intersection(v2v0, plane)
		float tCA = Intersection(v2, v0, plane);
		vec3 C = v2 + tCA * (v0 - v2);
		// return [Triangle(v1, B, v2), Triangle(B, v2, C)]
		triangles_vertices_camera.push_back(v1);
		triangles_vertices_camera.push_back(B);
		triangles_vertices_camera.push_back(v2);
		triangles_vertices_camera.push_back(B);
		triangles_vertices_camera.push_back(v2);
		triangles_vertices_camera.push_back(C);
		return triangles_vertices_camera;
	}
	return triangles_vertices_camera;
}



#include "Rasterizer.h"

#include <algorithm>

Fragment::Fragment() {}
Fragment::Fragment(const vec3 &position, const vec3 &normal, float w0, float w1, float w2) : position(position), normal(normal), w0(w0), w1(w1), w2(w2) {}

std::vector<Fragment> RasterizeFace(const std::vector<vec3>& face_vertices_in_screen_space, 
							  const std::vector<vec3>& vertex_normals_in_view_space, unsigned int width, unsigned int height) {
	std::vector<Fragment> fragments;
    // Compute Bounding box
	int min_x = width;
	int max_x = 0;
	int min_y = height;
	int max_y = 0;


for (const auto& vertex : face_vertices_in_screen_space) {
    int x = static_cast<int>(vertex.x());
    int y = static_cast<int>(vertex.y());
    min_x = std::min(min_x, x);
    max_x = std::max(max_x, x);
    min_y = std::min(min_y, y);
    max_y = std::max(max_y, y);
}

	// Compute edge functions
	vec3 v0 = face_vertices_in_screen_space[0];
	vec3 v1 = face_vertices_in_screen_space[1];
	vec3 v2 = face_vertices_in_screen_space[2];
	float area = EdgeFunction(v0, v1, v2);
	for (int y = min_y; y <= max_y; y++) {
		for (int x = min_x; x <= max_x; x++) {
			vec3 pixel(x + 0.5f, y + 0.5f, 0.0f);
			float w0 = EdgeFunction(v1, v2, pixel);
			float w1 = EdgeFunction(v2, v0, pixel);
			float w2 = EdgeFunction(v0, v1, pixel);
            bool inside = (w0 >= 0 && w1 >= 0 && w2 >= 0);
            if (area < 0) 
                inside = (w0 <= 0 && w1 <= 0 && w2 <= 0);
			if (inside) {
				w0 /= area;
				w1 /= area;
				w2 /= area;
				// Interpolate vertex attributes
				vec3 interpolated_normal = w0 * vertex_normals_in_view_space[0] +
										   w1 * vertex_normals_in_view_space[1] +
										   w2 * vertex_normals_in_view_space[2];
				// Interpolate vertex position
                vec3 interpolated_position = w0 * v0 + w1 * v1 + w2 * v2;
                // TODO Compute lighting
				// vec3 light_direction(0.0f, 0.0f, -1.0f);
				// float intensity = std::max(0.0f, dot(interpolated_normal.normalized(), light_direction));
                // Create a fragment
                Fragment fragment(interpolated_position, interpolated_normal, w0, w1, w2);
                fragments.push_back(fragment);
			}
		} // end for x
	} // end for y
    return fragments;
}


float EdgeFunction(const vec3& a, const vec3& b, const vec3& p) {
    vec3 A = p-a;
    vec3 B = b-a;
    return A.x() * B.y() - A.y() * B.x();
	// return (p.x() - a.x()) * (b.y() - a.y()) - (p.y() - a.y()) * (b.x() - a.x());
}

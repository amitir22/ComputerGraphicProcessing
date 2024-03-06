#include "Rasterizer.h"

#include <algorithm>

Fragment::Fragment() {}
Fragment::Fragment(const vec3 &position) : position(position) {}
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

// Function to sort vertices by y
void SortVerticesByY(vec3 &v0, vec3 &v1, vec3 &v2) {
    if (v1.y() < v0.y()) std::swap(v0, v1);
    if (v2.y() < v0.y()) std::swap(v0, v2);
    if (v2.y() < v1.y()) std::swap(v1, v2);
}

// Linear interpolation
float lerp(int start, int end, float t) {
    return start + (end - start) * t;
}

// Scanline fill between two points
void FillScanline(int y, int xStart, int xEnd) {
    for (int x = xStart; x <= xEnd; x++) {
        std::cout << "Pixel inside: (" << x << ", " << y << ")\n";
    }
}

std::vector<Fragment> RasterizeTriangle(vec3 v0, vec3 v1, vec3 v2) {

    std::vector<Fragment> fragments;
    // Sort vertices by y-coordinate
    SortVerticesByY(v0, v1, v2);

    // Rasterize each segment
    auto rasterizeSegment = [&](vec3 vStart, vec3 vEnd, bool isRight) {
        float dx = vEnd.x() - vStart.x();
        float dy = vEnd.y() - vStart.y();
        if (dy == 0) return;

        float dxdy = dx / dy;
        float x = vStart.x();
        int yStart = vStart.y();
        int yEnd = vEnd.y();

        for (int y = yStart; y <= yEnd; y++, x += dxdy) {
            if (isRight) {
                // fillScanline(y, lerp(v0.x(), v2.x(), float(y - v0.y()) / (v2.y() - v0.y())), x);
                int xStart = lerp(v0.x(), v2.x(), float(y - v0.y()) / (v2.y() - v0.y()));
                int xEnd = x;
                for (int x_idx = xStart; x_idx <= xEnd; x_idx++) {
                    fragments.push_back(Fragment(vec3(x_idx, y, 0), vec3(0, 0, 0), 0, 0, 0));
                }
            } else {
                // FillScanline(y, x, lerp(v0.x(), v2.x(), float(y - v0.y()) / (v2.y() - v0.y())));
                int xStart = x;
                int xEnd = lerp(v0.x(), v2.x(), float(y - v0.y()) / (v2.y() - v0.y()));
                for (int x_idx = xStart; x_idx <= xEnd; x_idx++) {
                    fragments.push_back(Fragment(vec3(x_idx, y, 0), vec3(0, 0, 0), 0, 0, 0));
                }
            }
        }
    };

    // Determine which side of the triangle is the longer one and rasterize accordingly
    bool isRight = (v1.x() - v0.x()) / (float)(v1.y() - v0.y()) > (v2.x() - v0.x()) / (float)(v2.y() - v0.y());
    rasterizeSegment(v0, v1, !isRight);
    rasterizeSegment(v1, v2, !isRight);
    return fragments;
}


// std::vector<Fragment> RasterizeFace_SSLOYSCAN(const std::vector<vec3>& face_vertices_in_screen_space, 
//                                     const std::vector<vec3>& vertex_normals_in_view_space,
//                                     unsigned int width, unsigned int height) {
//     std::vector<Fragment> fragments;

//     // Sorting the vertices based on their y-coordinate.
//     auto verts = face_vertices_in_screen_space; // Copy for sorting
//     std::sort(verts.begin(), verts.end(), [](const vec3& a, const vec3& b) {
//         return a.y() < b.y();
//     });
//     // avoid degenerate triangles
//     if (verts[0].y()==verts[1].y() && verts[0].y()==verts[2].y()) return fragments; 

//     int total_height = static_cast<int>(verts[2].y() - verts[0].y());
//     for (int i = 0; i <= total_height; i++) {
//         bool upper_half = i > verts[1].y() - verts[0].y() || verts[1].y() == verts[0].y();
//         int segment_height = upper_half ? static_cast<int>(verts[2].y() - verts[1].y()) : static_cast<int>(verts[1].y() - verts[0].y());
//         float alpha = static_cast<float>(i) / total_height;
//         float beta = segment_height > 0 ? static_cast<float>(i - (upper_half ? static_cast<int>(verts[1].y() - verts[0].y()) : 0)) / segment_height : 0;

//         vec3 A = verts[0] + (verts[2] - verts[0]) * alpha;
//         vec3 B = upper_half ? verts[1] + (verts[2] - verts[1]) * beta : verts[0] + (verts[1] - verts[0]) * beta;

//         Eigen::Vector2i A_screen(A.x(), A.y()), B_screen(B.x(), B.y());
//         if (A_screen.x() > B_screen.x()) std::swap(A_screen, B_screen);

//         for (int j = A_screen.x(); j <= B_screen.x(); j++) {
//             // Use the interpolated or vertex position for z-value
//             float z = A.z() + (B.z() - A.z()) * (j - A_screen.x()) / (B_screen.x() - A_screen.x());
//             fragments.emplace_back(vec3(j, verts[0].y() + i, z));
//         }
//     }

//     return fragments;
// }


// vec3 barycentric(Vec2i *pts, Vec2i P) { 
//     Vec3f u = Vec3f(pts[2][0]-pts[0][0], pts[1][0]-pts[0][0], pts[0][0]-P[0])^Vec3f(pts[2][1]-pts[0][1], pts[1][1]-pts[0][1], pts[0][1]-P[1]);
//     /* `pts` and `P` has integer value as coordinates
//        so `abs(u[2])` < 1 means `u[2]` is 0, that means
//        triangle is degenerate, in this case return something with negative coordinates */
//     if (std::abs(u.z)<1) return Vec3f(-1,1,1);
//     return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z); 
// } 

// std::vector<Fragment> RasterizeFaceSSLOYBary(const std::vector<vec3>& face_vertices_in_screen_space, 
//                                     const std::vector<vec3>& vertex_normals_in_view_space,
//                                     unsigned int width, unsigned int height) 
// {

// }

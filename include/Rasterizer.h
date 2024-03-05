#include "Geometry.h"

struct Fragment {
    vec3 position;
    vec3 normal;
    // barycentric coordinates
    float w0, w1, w2;
    // vec3 color; // TODO
    
    Fragment();
    Fragment(const vec3& position, const vec3& normal, float w0, float w1, float w2);
};


std::vector<Fragment> RasterizeFace(const std::vector<vec3>& face_vertices_in_screen_space, 
							  const std::vector<vec3>& vertex_normals_in_view_space,
                              unsigned int width, unsigned int height);

float EdgeFunction(const vec3& a, const vec3& b, const vec3& c);


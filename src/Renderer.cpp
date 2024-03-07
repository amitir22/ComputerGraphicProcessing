// Renderer.cpp
#include "Renderer.h" 

#include <algorithm> // for std::swap, 
#include <cmath>   // for std::abs

#include "Constants.h" // for cg::constants::SCR_WIDTH, cg::constants::SCR_HEIGHT
#include "Geometry.h"
//#include "Rasterizer.h"

#define INDEX(width,x,y,c) (x+y*width)*3+c

Renderer::Renderer() : Renderer(cg::constants::SCR_WIDTH, cg::constants::SCR_HEIGHT) {};

Renderer::Renderer(int width, int height) : width_(width), height_(height) {
	framebuffer_ = std::make_unique<GLubyte[]>(width_ * height_ * 3);
	z_buffer_ = std::make_unique<float[]>(width_ * height_);
	// fill z_buffer with z_far
	std::fill(z_buffer_.get(), z_buffer_.get() + width_ * height_, z_far);

	model_transform_ = mat4::Identity();
	view_transform_ = mat4::Identity();
	projection_transform_ = mat4::Identity();
	view_port_transform_ = geometry::getViewPortTransform(width_, height_);
	normal_transform_ = mat3::Identity();
	is_perspective_ = false;
}

void Renderer::HandleWindowReshape(int new_width, int new_height) {
	width_ = new_width;
	height_ = new_height;
	framebuffer_.reset(new GLubyte[width_ * height_ * 3]);
	z_buffer_.reset(new float[width_ * height_]);
	view_port_transform_ = geometry::getViewPortTransform(width_, height_);
}

void Renderer::ClearBuffers()
{
	memset(framebuffer_.get(), 0, width_ * height_ * 3); // Initialize with black color
	memset(z_buffer_.get(), z_far, width_ * height_ * sizeof(float)); // Initialize with z_far
}

/////////////////////////////////////////////////////
//				DRAW FUNCTIONS
///////////////////////////////////////////////////
float EdgeFunction(const vec3& a, const vec3& b, const vec3& c) {
	return (c.x() - a.x()) * (b.y() - a.y()) - (c.y() - a.y()) * (b.x() - a.x());
}

int GetIntMin(float a, float b, float c) { return static_cast<int>(std::floor(std::min(a, std::min(b, c))));}
int GetIntMax(float a, float b, float c) {return static_cast<int>(std::ceil(std::max(a, std::max(b, c))));}
void Renderer::DrawScene(Scene *scene)
{
	ClearBuffers();
	// Set scene and transformations
	SetScene(scene); 
	// Draw models
	std::vector<MeshModel*> models = scene->GetModels();
	for (const auto& model : models) {
		// set transforms
		model_transform_ =  model->GetModelTransform();
		// TODO get model material
		mat4 modelview_matrix = view_transform_ * model_transform_;
		normal_transform_ = geometry::getNormalTransfrom(modelview_matrix);
		// Get model data
		mvp_ = projection_transform_ * modelview_matrix;
		Eigen::MatrixXf vertices_local = model->GetVerticesLocal(); // (4, 3N)
		Eigen::MatrixXf normals_local = model->GetNormalsLocal(); // (3, 3N)
		Eigen::MatrixXf face_normals_local = model->GetFaceNormalsLocal(); // (3, N)

		Eigen::MatrixXf vertices_clip = mvp_ * vertices_local; // (4, 3N)
		Eigen::MatrixXf vertices_ndc = vertices_clip.topRows(3); // (3, 3N)
		if (is_perspective_)
			vertices_ndc = vertices_ndc.array().rowwise() / vertices_clip.row(3).array();

		// RenderTriangle For each face
		for (int i = 0; i < face_normals_local.cols(); i++) 
		{
			// Backface culling
			vec3 face_normal = face_normals_local.col(i);
			float z_dot = scene_->GetActiveCamera()->forward.dot(face_normal);
			if (z_dot >= 0)
				continue;
			// extract v0, v1,v2 from vertices_ndc
			Eigen::Vector3f v0_raster = vertices_ndc.col(i * 3);
			Eigen::Vector3f v1_raster = vertices_ndc.col(i * 3 + 1);
			Eigen::Vector3f v2_raster = vertices_ndc.col(i * 3 + 2);
			// if all vertices share same y value - continue
			if (v0_raster.y() == v1_raster.y() && v0_raster.y() == v2_raster.y()) continue;
			// Compute bounding box
			int min_x = GetIntMin(v0_raster.x(), v1_raster.x(), v2_raster.x());
			int max_x = GetIntMax(v0_raster.x(), v1_raster.x(), v2_raster.x());
			int min_y = GetIntMin(v0_raster.y(), v1_raster.y(), v2_raster.y());
			int max_y = GetIntMax(v0_raster.y(), v1_raster.y(), v2_raster.y());
			// Check if triangle is out of screen
			if (max_x < 0 || min_x > width_ || max_y < 0 || min_y > height_)
				continue;
			// Clip bounding box to screen
			uint32_t x0 = std::max(int32_t(0), (int32_t)(std::floor(min_x)));
			uint32_t x1 = std::min(int32_t(width_) - 1, (int32_t)(std::floor(max_x)));
			uint32_t y0 = std::max(int32_t(0), (int32_t)(std::floor(min_y)));
			uint32_t y1 = std::min(int32_t(height_) - 1, (int32_t)(std::floor(max_y)));
			// compute area with an edge function
			float area = EdgeFunction(v0_raster, v1_raster, v2_raster);
			for (uint32_t y = y0; y <= y1; ++y)
			{
				for (uint32_t x = x0; x <= x1; ++x) {
					vec3 pixel(x + 0.5, y + 0.5, 0);
					float w0 = EdgeFunction(v1_raster, v2_raster, pixel);
					float w1 = EdgeFunction(v2_raster, v0_raster, pixel);
					float w2 = EdgeFunction(v0_raster, v1_raster, pixel);
					bool inside_ = (w0 >= 0 && w1 >= 0 && w2 >= 0);
					if (area < 0)
						inside_ = (w0 <= 0 && w1 <= 0 && w2 <= 0);
					if (inside_) {
						w0 /= area;
						w1 /= area;
						w2 /= area;
						float oneOverZ = v0_raster.z() * w0 + v1_raster.z() * w1 + v2_raster.z() * w2;
						float z = 1 / oneOverZ;
						// Depth-buffer test
						if (z < z_buffer_[y * width_ + x]) {
							z_buffer_[y * width_ + x] = z;
							// TODO fragment shader - compute color, according to selected lighting method
							// Renderer::getShadingForFragment(Face, fragment)
							DrawPixel(x, y);
						} // end if depth-buffer test
					} // end if inside
				} // end for x
			} // end for y
		} // end for each Triangle
	} // end for each model
}


//void Renderer::RenderFace(vec3 v0, vec3 v1, vec3 v2, vec3 n0, vec3 n1, vec3 n2, vec3 face_normal){
//	// TODO Backface culling
//	// Project vertices to view-space using view_transform_ * model_transform_ 
//	std::vector<vec4> vertices_in_view_space(face.vertices.size());
//	std::vector<vec3> vertex_normals_in_view_space(face.vertices.size());
//	// project vertices to view space
//	for (int i = 0; i < face.vertices.size(); i++) {
//		vec4 h_vertex_in_local_space = homogoneizeVector(face.vertices[i].position_in_local_space);
//		vertices_in_view_space[i] = view_transform_ * model_transform_ * h_vertex_in_local_space;
//		vertex_normals_in_view_space[i] = normal_transform_ * face.vertices[i].normal_in_local_space;
//	}
//	
//	// Project vertices to screen space
//	std::vector<vec3> vertices_in_screen_space(face.vertices.size());
//	for (int i = 0; i < face.vertices.size(); i++) {
//		// Projection 
//		vec4 vertex_in_clip_space = projection_transform_ * vertices_in_view_space[i];
//		// Perspective divide
//		if (is_perspective_)
//			vertex_in_clip_space /= vertex_in_clip_space.w();
//		// Viewport transform
//		vec4 vertex_in_screen_space = view_port_transform_ * vertex_in_clip_space;
//		vertices_in_screen_space[i] = vec3(vertex_in_screen_space.x(), vertex_in_screen_space.y(), vertex_in_screen_space.z());
//	}
//	// TODO (Possible) Clipping
//	
//	// Rasterization
//	// std::vector<Fragment> fragments = RasterizeFace(vertices_in_screen_space, vertex_normals_in_view_space, width_, height_);
//	RasterizeFace_SSLOYSCAN(vertices_in_screen_space, vertex_normals_in_view_space, width_, height_);
//	// std::vector<Fragment> fragments = RasterizeFace_SSLOYSCAN(vertices_in_screen_space, vertex_normals_in_view_space, width_, height_);
//	// std::vector<Fragment> fragments = RasterizeTriangle(vertices_in_screen_space[0], vertices_in_screen_space[1], vertices_in_screen_space[2]);
//	// Fragment shading
//	// for (const auto& fragment : fragments) {
//		// depth-testing
//		// Interpolate vertex attributes
//		// Compute lighting
//		// Texture mapping
//		// Draw Pixel 
//		// DrawPixel(static_cast<int>(fragment.position.x()), static_cast<int>(fragment.position.y()));
//	// }
//}
//
//void Renderer::RasterizeFace_SSLOYSCAN(const std::vector<vec3>& face_vertices_in_screen_space, 
//                                    const std::vector<vec3>& vertex_normals_in_view_space,
//                                    unsigned int width, unsigned int height) {
//	// Sorting the vertices based on their y-coordinate.
//    auto verts = face_vertices_in_screen_space; // Copy for sorting
//    std::sort(verts.begin(), verts.end(), [](const vec3& a, const vec3& b) {
//        return a.y() < b.y();
//    });
//    // avoid degenerate triangles
//    if (verts[0].y()==verts[1].y() && verts[0].y()==verts[2].y()) return; 
//
//    int total_height = static_cast<int>(verts[2].y() - verts[0].y());
//    for (int i = 0; i <= total_height; i++) {
//        bool upper_half = i > verts[1].y() - verts[0].y() || verts[1].y() == verts[0].y();
//        int segment_height = upper_half ? static_cast<int>(verts[2].y() - verts[1].y()) : static_cast<int>(verts[1].y() - verts[0].y());
//        float alpha = static_cast<float>(i) / total_height;
//        float beta = segment_height > 0 ? static_cast<float>(i - (upper_half ? static_cast<int>(verts[1].y() - verts[0].y()) : 0)) / segment_height : 0;
//
//        vec3 A = verts[0] + (verts[2] - verts[0]) * alpha;
//        vec3 B = upper_half ? verts[1] + (verts[2] - verts[1]) * beta : verts[0] + (verts[1] - verts[0]) * beta;
//
//        Eigen::Vector2i A_screen(A.x(), A.y()), B_screen(B.x(), B.y());
//        if (A_screen.x() > B_screen.x()) std::swap(A_screen, B_screen);
//
//        for (int j = A_screen.x(); j <= B_screen.x(); j++) {
//            // Use the interpolated or vertex position for z-value
//            // float z = A.z() + (B.z() - A.z()) * (j - A_screen.x()) / (B_screen.x() - A_screen.x());
//            DrawPixel(j, verts[0].y() + i);
//        }
//    }
//}
//		


void Renderer::DrawPixel(int x, int y) {
	if (x < 0 || x >= width_ || y < 0 || y >= height_) {
		return;
	}
	framebuffer_[INDEX(width_, x, y, 0)] = 255;
	framebuffer_[INDEX(width_, x, y, 1)] = 255;
	framebuffer_[INDEX(width_, x, y, 2)] = 255;
}

void Renderer::SetScene(Scene *scene)
{
	this->scene_ = scene;
	view_transform_ = scene->GetActiveCamera()->GetViewTransform();
	projection_transform_ = scene->GetActiveCamera()->GetProjectionTransform();
	is_perspective_ = scene->GetActiveCamera()->IsPerspectiveProjection();

	// TODO get light

	z_near = scene->GetActiveCamera()->z_near_;
	z_far = scene->GetActiveCamera()->z_far_;
	right = scene->GetActiveCamera()->right_;
	top = scene->GetActiveCamera()->top_;
}

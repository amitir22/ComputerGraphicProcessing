// Renderer.cpp
#include "Renderer.h" 

#include <algorithm> // for std::swap, 
#include <cmath>   // for std::abs

#include "Constants.h" // for cg::constants::SCR_WIDTH, cg::constants::SCR_HEIGHT
#include <set>
// #include <Eigen/src/Core/MatrixBase.h>
#include "Geometry.h"

#define INDEX(width,x,y,c) (x+y*width)*3+c

Renderer::Renderer() : Renderer(cg::constants::SCR_WIDTH, cg::constants::SCR_HEIGHT) {};

Renderer::Renderer(int width, int height) : width_(width), height_(height) {
	framebuffer_ = std::make_unique<GLubyte[]>(width_ * height_ * 3);
	z_buffer_ = std::make_unique<float[]>(width_ * height_);

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

void Renderer::SetViewTransform(const mat4& view_transform)
{
	view_transform_ = view_transform;
}

void Renderer::SetProjectionTransform(const mat4& projection, bool is_perspective)
{
	projection_transform_ = projection;
	is_perspective_ = is_perspective;
}

void Renderer::SetModelTransform(const mat4& model_transform)
{
	model_transform_ = model_transform;
}

void Renderer::ClearBuffers()
{
	memset(framebuffer_.get(), 0, width_ * height_ * 3); // Initialize with black color
	memset(z_buffer_.get(), 0, width_ * height_ * sizeof(float)); // Initialize with 0
}

/////////////////////////////////////////////////////
//				DRAW FUNCTIONS
///////////////////////////////////////////////////
void Renderer::DrawScene(Scene *scene)
{
	// Clear buffers
	ClearBuffers();
	// Set scene
	SetScene(scene);
	// Draw models
	std::vector<MeshModel*> models = scene->GetModels();
	for (const auto& model : models) {
		// set model transforms
		SetModelTransform(model->GetModelTransform());
		// Compute modelview matrix
		mat4 modelview_matrix = view_transform_ * model_transform_;
		// Set normal_transform_ to transpose of inverse of modelview matrix
		normal_transform_ = geometry::getNormalTransfrom(modelview_matrix);
		// Get model faces
		std::vector<Face> faces = model->GetFaces();
		// Render each face
		for (const auto& face : faces) {
			RenderFace(face);
		}
	}
}

void Renderer::RenderFace(const Face& face){
	// TODO Backface culling
	// Project vertices to view-space using view_transform_ * model_transform_ 
	std::vector<vec4> vertices_in_view_space(face.vertices.size());
	std::vector<vec3> vertex_normals_in_view_space(face.vertices.size());
	vec3 face_normal_in_view_space = (normal_transform_ * face.normal_).normalized();
	for (int i = 0; i < face.vertices.size(); i++) {
		vec4 temp_vertex = homogoneize_vector(face.vertices[i].position_in_local_coords);
		vertices_in_view_space[i] = view_transform_ * model_transform_ * temp_vertex;
		vertex_normals_in_view_space[i] = normal_transform_ * face.vertices[i].normal_in_local_coords;
	}
	
	// Project vertices to screen space
	std::vector<vec3> vertices_in_screen_space(face.vertices.size());
	for (int i = 0; i < face.vertices.size(); i++ ) {
		// Projection 
		vec4 vertex_in_clip_space = projection_transform_ * vertices_in_view_space[i];
		// Perspective divide
		if (is_perspective_)
			vertex_in_clip_space /= vertex_in_clip_space.w();
		// Viewport transform
		vec4 temp_vertex = view_port_transform_ * vertex_in_clip_space;
		vertices_in_screen_space[i] = vec3(temp_vertex.x(), temp_vertex.y(), temp_vertex.z());
	}
	// (Possible) Clipping
	
	// Rasterization
	RasterizeFace(vertices_in_screen_space, vertex_normals_in_view_space);
		// Bounding box calculation
		// Compute edge functions
		
	// Fragment shading
		// depth-testing
		// Interpolate vertex attributes
		// Compute lighting
		// Texture mapping
}

void Renderer::RasterizeFace(const std::vector<vec3>& face_vertices_in_screen_space, 
							  const std::vector<vec3>& vertex_normals_in_view_space) {
	// Compute Bounding box
	int min_x = width_;
	int max_x = 0;
	int min_y = height_;
	int max_y = 0;
	for (const auto& vertex : face_vertices_in_screen_space) {
		int x = static_cast<int>(vertex.x());
		int y = static_cast<int>(vertex.y());
		min_x = std::min(min_x, x);
		max_x = std::max(max_x, x);
		min_y = std::min(min_y, y);
		max_y = std::max(max_y, y);
	}

	// Clip bounding box to screen boundaries
	min_x = std::max(min_x, 0);
	max_x = std::min(max_x, width_ - 1);
	min_y = std::max(min_y, 0);
	max_y = std::min(max_y, height_ - 1);

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
			if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
				w0 /= area;
				w1 /= area;
				w2 /= area;
				// Interpolate vertex attributes
				vec3 interpolated_normal = w0 * vertex_normals_in_view_space[0] +
										   w1 * vertex_normals_in_view_space[1] +
										   w2 * vertex_normals_in_view_space[2];
				// Compute lighting
				// vec3 light_direction(0.0f, 0.0f, -1.0f);
				// float intensity = std::max(0.0f, dot(interpolated_normal.normalized(), light_direction));
				// // Fragment shading
				// DrawPixel(x, y, intensity);
				DrawPixel(x,y);
			}
		}
	}
}

float Renderer::EdgeFunction(const vec3& a, const vec3& b, const vec3& c) {
	return (c.x() - a.x()) * (b.y() - a.y()) - (c.y() - a.y()) * (b.x() - a.x());
}

void Renderer::DrawPixel(int x, int y, float intensity) {
	if (x < 0 || x >= width_ || y < 0 || y >= height_) {
		return;
	}
	GLubyte color = static_cast<GLubyte>(intensity * 255);
	framebuffer_[INDEX(width_, x, y, 0)] = color;
	framebuffer_[INDEX(width_, x, y, 1)] = color;
	framebuffer_[INDEX(width_, x, y, 2)] = color;
}

void Renderer::DrawTriangles(const std::vector<vec3>* vertices, const std::vector<vec3>* normals) {
	// Draw a point right at the center of screen
	if (vertices == nullptr || vertices->size() % 3 != 0) {
		throw std::runtime_error("Invalid vertices input.");
	}
	mat4 mvp = projection_transform_ * view_transform_ * model_transform_;
	// Transform vertices from local space to screen space, save them in a new vector
	// create a vector of screen vertices initialized to zero of size vertices->size()
	std::vector<vec4> screen_vertices(vertices->size());
	for (int i = 0; i < vertices->size(); i++) {
		vec4 local_vertex{ (*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z(), 1 };
		vec4 clip_vertex = mvp * local_vertex; // normalized device coordinates
		if (is_perspective_)
			clip_vertex /= clip_vertex.w();
		screen_vertices[i] = view_port_transform_ * clip_vertex; // pixel coordinates
	}

	// Draw each triangle
	for (int i = 0; i < screen_vertices.size(); i += 3) {
		// call drawLine and cast the vertices to int
		DrawLine((int)screen_vertices[i].x(), (int)screen_vertices[i].y(),
			(int)screen_vertices[i + 1].x(), (int)screen_vertices[i + 1].y());
		DrawLine((int)screen_vertices[i + 1].x(), (int)screen_vertices[i + 1].y(),
			(int)screen_vertices[i + 2].x(), (int)screen_vertices[i + 2].y());
		DrawLine((int)screen_vertices[i + 2].x(), (int)screen_vertices[i + 2].y(),
			(int)screen_vertices[i].x(), (int)screen_vertices[i].y());
	}
}

void Renderer::DrawLine(int x0, int y0, int x1, int y1) {
	const bool steep = abs(y1 - y0) > abs(x1 - x0);
	// If slope (in absolute value) is larger than 1, we switch roles of x and y 
	if (steep) {
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	// Make sure we draw from left to right
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	const int dx = x1 - x0;
	const int dy = std::abs(y1 - y0); // Also handles negative slopes
	const int ystep = (y0 < y1) ? 1 : -1;
	int D = 2 * dy - dx;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (x >= width_ || y >= height_ || y < 0) {
			return;
		}
		if (steep) {
			DrawPixel(y, x);
		}
		else {
			DrawPixel(x, y);
		}
		if (D > 0) { // If D > 0, we should move one step in the y direction
			y += ystep;
			D += 2 * (dy - dx);
		}
		else { // Else, don't increment
			D += 2 * dy;
		}
	}
}

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
	scene = scene;
	view_transform_ = scene->GetActiveCamera()->GetViewTransform();
	projection_transform_ = scene->GetActiveCamera()->GetProjectionTransform();
	is_perspective_ = scene->GetActiveCamera()->IsPerspectiveProjection();
}

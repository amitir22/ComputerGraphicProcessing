// Renderer.cpp
#include "Renderer.h" 

#include <algorithm> // for std::swap, 
#include <cmath>   // for std::abs

#include "Constants.h" // for cg::constants::SCR_WIDTH, cg::constants::SCR_HEIGHT

#define INDEX(width,x,y,c) (x+y*width)*3+c

Renderer::Renderer() : Renderer(cg::constants::SCR_WIDTH, cg::constants::SCR_HEIGHT) {};

Renderer::Renderer(int width, int height) : width_(width), height_(height) {
	framebuffer_ = new GLubyte[width_ * height_ * 3];
	// TODO init z-buffer
	model_transform_ = mat4::Identity();
	view_transform_ = mat4::Identity();
	projection_transform_ = mat4::Identity();
	view_port_transform_ = Geometry::getViewPortTransform(width_, height_);
	normal_transform_ = mat3::Identity();
	is_perspective_ = false;
}

void Renderer::HandleWindowReshape(int new_width, int new_height) {
	width_ = new_width;
	height_ = new_height;
	delete framebuffer_;
	framebuffer_ = new GLubyte[width_ * height_ * 3];
	view_port_transform_ = Geometry::getViewPortTransform(width_, height_);
}
/////////////////////////////////////////////////////
//				DRAW FUNCTIONS
///////////////////////////////////////////////////
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

void Renderer::SetViewTransform(const mat4& view_transform)
{
	view_transform_ = view_transform;
}

void Renderer::SetProjectionTransform(const mat4& projection, bool is_perspective)
{
	projection_transform_ = projection;
	is_perspective_ = is_perspective;
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
	int res = INDEX(width_, x, y, 2);
	// do this with a try and except
	if (res < 0 || res >= width_ * height_ * 3) {
		return;
	}
	framebuffer_[INDEX(width_, x, y, 0)] = 255;
	framebuffer_[INDEX(width_, x, y, 1)] = 255;
	framebuffer_[INDEX(width_, x, y, 2)] = 255;
}


void Renderer::SetModelTransform(const mat4& model_transform, const mat3& normal_transform)
{
	model_transform_ = model_transform;
	normal_transform_ = normal_transform;
}

void Renderer::ClearBuffers()
{
	memset(framebuffer_, 0, width_ * height_ * 3); // Initialize with black color
	// TODO clear z_buffer_
}

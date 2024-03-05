// Renderer.cpp
#include "Renderer.h" 

#include <algorithm> // for std::swap, 
#include <cmath>   // for std::abs

#include "Constants.h" // for cg::constants::SCR_WIDTH, cg::constants::SCR_HEIGHT
#include "Geometry.h"
#include "Rasterizer.h"

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
		// set transforms
		model_transform_ =  model->GetModelTransform();
		mat4 modelview_matrix = view_transform_ * model_transform_;
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
		vec4 h_vertex_in_local_space = homogoneize_vector(face.vertices[i].position_in_local_space);
		vertices_in_view_space[i] = view_transform_ * model_transform_ * h_vertex_in_local_space;
		vertex_normals_in_view_space[i] = normal_transform_ * face.vertices[i].normal_in_local_space;
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
		vec4 vertex_in_screen_space = view_port_transform_ * vertex_in_clip_space;
		vertices_in_screen_space[i] = vec3(vertex_in_screen_space.x(), vertex_in_screen_space.y(), vertex_in_screen_space.z());
	}
	// TODO (Possible) Clipping
	
	// Rasterization
	std::vector<Fragment> fragments = RasterizeFace(vertices_in_screen_space, vertex_normals_in_view_space, width_, height_);
	// Fragment shading
	for (const auto& fragment : fragments) {
		// depth-testing
		// Interpolate vertex attributes
		// Compute lighting
		// Texture mapping
		// Draw Pixel 
		DrawPixel(static_cast<int>(fragment.position.x()), static_cast<int>(fragment.position.y()));
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
	this->scene_ = scene;
	view_transform_ = scene->GetActiveCamera()->GetViewTransform();
	projection_transform_ = scene->GetActiveCamera()->GetProjectionTransform();
	is_perspective_ = scene->GetActiveCamera()->IsPerspectiveProjection();
}

// Renderer.h
#pragma once
#include <memory>
#include <vector>

#include "glad/glad.h"

#include "Geometry.h" // For viewport transformation
#include "Scene.h"

class Renderer
{
public:
	std::unique_ptr<GLubyte[]> framebuffer_;
	std::unique_ptr<float[]> z_buffer_;
	unsigned int width_, height_;

	// Transformation Matrices
	mat4 model_transform_;
	mat4 view_transform_; 
	mat4 projection_transform_; 
	mat4 mvp_;
	bool is_perspective_; 
	mat4 viewport_transform_; 
	mat3 normal_transform_; 
	Eigen::MatrixXf normals_end_vertices_raster;
	// Constructors
	Renderer();
	Renderer(int width, int height);
	
	// Draw functions
	void DrawScene(Scene* scene);
    void DrawLine(int x0, int y0, int x1, int y1);
    void DrawPixel(int x, int y);
	// Utils
	void ClearBuffers();
	void HandleWindowReshape(int new_width, int new_height);


	// TODO delete
	float z_near;
	float z_far;
	float right;
	float top;

	bool show_vertex_normals_ = false;
	bool show_face_normals_ = false;
	
private:
	Scene* scene_;
	// set scene
	void SetScene(Scene* scene);
};

// Flat uniform for each polygon
// Gourard interpolate to the inside
// Phong calculate for each fragment
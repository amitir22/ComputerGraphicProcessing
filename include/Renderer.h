// Renderer.h
#pragma once
#include <vector>
#include <memory>
#include "glad/glad.h"
#include "CG.h"
#include "vec.h"
#include "mat.h"
#include "Geometry.h" // For viewport

class Renderer
{
public:
	//std::unique_ptr<float[]> framebuffer_;
	GLubyte* framebuffer_;
	std::unique_ptr<float[]> z_buffer_;
	int width_, height_;

	// Transformation Matrices
	mat4 model_transform_;
	mat4 view_transform_;
	mat4 projection_transform_;
	bool is_perspective_;
	mat4 view_port_transform_;
	mat3 normal_transform_;

	Renderer();
	Renderer(int width, int height);
	void DrawTriangles(const std::vector<vec3>* vertices, const std::vector<vec3>* normals = NULL);
	void SetModelTransform(const mat4& model_transform, const mat3& normal_transform);
	void SetViewTransform(const mat4& view_transform);
	void SetProjectionTransform(const mat4& projection, bool is_perspective);
	void ClearBuffers();
	void HandleWindowReshape(int new_width, int new_height);
	// Our functions
	void DrawLine(int x0, int y0, int x1, int y1);
	void DrawPixel(int x, int y);
private:
};

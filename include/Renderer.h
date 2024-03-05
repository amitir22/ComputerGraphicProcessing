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
	int width_, height_;

	// Transformation Matrices
	mat4 model_transform_; // TODO Maybe delete, replace with Model.transform()
	mat4 view_transform_; // TODO delete, replace with Camera.toViewSpace()
	mat4 projection_transform_; // TODO delete, reaplce with Camera.project()
	bool is_perspective_; // TODO delete, replace with Camera.isPerspective()
	mat4 view_port_transform_; 
	mat3 normal_transform_; // TODO delete, replace some Geometry call, that gets camera and model and retruns normal_transform

	Renderer();
	Renderer(int width, int height);
	
	void DrawScene(Scene* scene);
    void RenderFace(const Face &face);
    void RasterizeFace(const std::vector<vec3> &face_vertices_in_screen_space, const std::vector<vec3> &vertex_normals_in_view_space);
    float EdgeFunction(const vec3 &a, const vec3 &b, const vec3 &c);
    void DrawPixel(int x, int y, float intensity);
    void DrawTriangles(const std::vector<vec3> *vertices, const std::vector<vec3> *normals = NULL);
    void SetModelTransform(const mat4& model_transform);
	void SetViewTransform(const mat4& view_transform);
	void SetProjectionTransform(const mat4& projection, bool is_perspective);
	void ClearBuffers();
	void HandleWindowReshape(int new_width, int new_height);
	// Our functions
	void DrawLine(int x0, int y0, int x1, int y1);
	void DrawPixel(int x, int y);
private:
	Scene* scene;
	// set scene
	void SetScene(Scene* scene);
};
// Flat uniform for each polygon
// Gourard interpolate to the inside
// Phong calculate for each fragment
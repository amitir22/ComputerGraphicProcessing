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
	mat4 model_transform_; // TODO Maybe delete, replace with Model.transform()
	mat4 view_transform_; // TODO delete, replace with Camera.toViewSpace()
	mat4 projection_transform_; // TODO delete, reaplce with Camera.project()
	bool is_perspective_; // TODO delete, replace with Camera.isPerspective()
	mat4 view_port_transform_; 
	mat3 normal_transform_; // TODO delete, replace some Geometry call, that gets camera and model and retruns normal_transform

	// Constructors
	Renderer();
	Renderer(int width, int height);
	
	// Draw functions
	void DrawScene(Scene* scene);
    void RenderFace(const Face &face);
    void DrawPixel(int x, int y);
	// Utils
	void ClearBuffers();
	void HandleWindowReshape(int new_width, int new_height);
	
private:
	Scene* scene_;
	// set scene
	void SetScene(Scene* scene);
};

// Flat uniform for each polygon
// Gourard interpolate to the inside
// Phong calculate for each fragment
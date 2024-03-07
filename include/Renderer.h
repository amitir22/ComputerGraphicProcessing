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
	mat4 mvp_;
	bool is_perspective_; // TODO delete, replace with Camera.isPerspective()
	mat3 view_port_transform_; 
	mat3 normal_transform_; // TODO delete, replace some Geometry call, that gets camera and model and retruns normal_transform

	// Constructors
	Renderer();
	Renderer(int width, int height);
	
	// Draw functions
	void DrawScene(Scene* scene);
    void RenderFace(vec3 v0, vec3 v1, vec3 v2, vec3 n0, vec3 n1, vec3 n2, vec3 face_normal);
	void RenderFaceScratch(vec3 v0, vec3 v1, vec3 v2, vec3 n0, vec3 n1, vec3 n2, vec3 face_normal);
    void DrawPixel(int x, int y);
	void RasterizeFace_SSLOYSCAN(const std::vector<vec3>& face_vertices_in_screen_space, 
                                    const std::vector<vec3>& vertex_normals_in_view_space,
                                    unsigned int width, unsigned int height);
	// Utils
	void ClearBuffers();
	void HandleWindowReshape(int new_width, int new_height);


	// TODO delete
	float z_near;
	float z_far;
	float right;
	float top;
	
private:
	Scene* scene_;
	// set scene
	void SetScene(Scene* scene);
};

// Flat uniform for each polygon
// Gourard interpolate to the inside
// Phong calculate for each fragment
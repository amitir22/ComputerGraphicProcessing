// Renderer.h
#pragma once
#include <memory>
#include <vector>

#include "glad/glad.h"

#include "Geometry.h" // For viewport transformation
#include "Scene.h"
#include "MyRGB.h"
#include "Clipper.h"


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
	FrustumClipper frustum_clipper_;
	// Constructors
	Renderer();
	Renderer(int width, int height);
	
	// Draw functions
	void DrawScene(Scene* scene);
	//vec3 ProjectPoint(const vec3& v_world);
	vec3 ProjectPoint(const vec4& v_world);
	void Draw_GL_LINE(const vec4& v0_world, const vec4& v1_world, MyRGB color = WHITE);
	void Draw_GL_LINE(const vec3& v0_world, const vec3& v1_world, MyRGB color = WHITE);
	void Draw_GL_LINE_STRIP(const matxf& vertices_world, MyRGB color = WHITE);
	void Draw_GL_LINE_LOOP(const matxf& vertices_world, MyRGB color = WHITE); // Like Draw_GL_LINE_STRIP, but we also draw a line from the last vertex to the first
	void DrawAxes();
	void DrawMeshModel(MeshModel* model);
	void DrawLine(const vec3& v0, const vec3& v1, MyRGB color = WHITE);
	void DrawLine(int x0, int y0, float z0, int x1, int y1, float z1, MyRGB color = WHITE);
	void DrawPixel(int x, int y, float z, MyRGB color = WHITE, bool do_depth_test = true);
	void DrawPixel(const vec3& v, MyRGB color = WHITE) { DrawPixel(static_cast<int>(v.x()), static_cast<int>(v.y()), v.z(), color); }
	
	// Utils
	void ClearBuffers();
	void HandleWindowReshape(int new_width, int new_height);

	// TODO delete
	float z_near_, z_far_;
	float canvas_top_, canvas_right_;

	// Flags
	bool show_vertex_normals_ = false;
	bool show_face_normals_ = false;
	bool show_cameras_ = false;
	bool show_bounding_box_ = false;
	bool show_wireframe_ = true;
	bool show_lights_ = false;
	bool is_backface_culling_ = false;
	bool is_clipping_ = false;
	bool show_axes_ = true;
	ShadingType selected_shading_type = WHITE_SHADING;

	vec3 center_of_mass_camera_debug = vec3(0, 0, 0); // delete
	
private:
	Scene* scene_;
	// set scene
	void SetScene(Scene* scene);
};

// Flat uniform for each polygon
// Gourard interpolate to the inside
// Phong calculate for each fragment
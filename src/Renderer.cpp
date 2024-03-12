// Renderer.cpp
#include "Renderer.h" 

#include <algorithm> // for std::swap, 
#include <cmath>   // for std::abs

#include "Clipper.h"
#include "Constants.h" // for cg::constants::SCR_WIDTH, cg::constants::SCR_HEIGHT
#include "Fragment.h"
#include "Geometry.h"
#include "RenderUtils.h" // for EdgeFunction, GetIntMin, GetIntMax


#define INDEX(width,x,y,c) (x+y*width)*3+c

Renderer::Renderer() : Renderer(cg::constants::SCR_WIDTH, cg::constants::SCR_HEIGHT) {};

Renderer::Renderer(int width, int height) : width_(width), height_(height) {
	framebuffer_ = std::make_unique<GLubyte[]>(width_ * height_ * 3);
	z_buffer_ = std::make_unique<float[]>(width_ * height_);

	// TODO: maybe put under condition if (draw_bloom_)
	bloom_blur_byte_buffer_ = make_unique<GLubyte[]>(width_ * height_ * 3);
	bloom_pre_blur_buffer_ = make_unique<vec3[]>(width_ * height_);
	bloom_post_blur_buffer_ = make_unique<vec3[]>(width_ * height_);
	// fill z_buffer with z_far
	std::fill(z_buffer_.get(), z_buffer_.get() + width_ * height_, z_far_);

	model_transform_ = mat4::Identity();
	view_transform_ = mat4::Identity();
	projection_transform_ = mat4::Identity();
	viewport_transform_ = geometry::GetViewportTransform(width_, height_);
	normal_transform_ = mat3::Identity();
	is_perspective_ = false;
}

void Renderer::HandleWindowReshape(int new_width, int new_height) {
	width_ = new_width;
	height_ = new_height;
	framebuffer_.reset(new GLubyte[width_ * height_ * 3]);
	// fill z_buffer with z_far
	z_buffer_.reset(new float[width_ * height_]);
	std::fill(z_buffer_.get(), z_buffer_.get() + width_ * height_, z_far_);
	viewport_transform_ = geometry::GetViewportTransform(width_, height_);
	Scene* scene = scene_;
	// For every camera in scene, call camera.HandleWindowReshape(new_width, new_height)
	for (auto& camera : scene->GetCameras()) {
		camera->HandleWindowReshape(new_width, new_height);
	}
}

void Renderer::ClearBuffers()
{
	memset(framebuffer_.get(), 0, width_ * height_ * 3); // Initialize with black color
	memset(z_buffer_.get(), z_far_, width_ * height_ * sizeof(float)); // Initialize with z_far
}

void Renderer::SetScene(Scene *scene)
{
	this->scene_ = scene;
	Camera* camera = scene->GetActiveCamera();
	view_transform_ = camera->GetViewTransform();
	projection_transform_ = camera->GetProjectionTransform();
	is_perspective_ = camera->IsPerspectiveProjection();

	// TODO get light
	z_near_ = camera->GetZNear();
	z_far_ = camera->GetZFar();
	camera->GetCanvasShape(canvas_top_, canvas_right_);

	if (this->is_clipping_) {
		frustum_clipper_.SetCamera(camera);
	}
}

/////////////////////////////////////////////////////
//				DRAW FUNCTIONS
///////////////////////////////////////////////////

void Renderer::DrawScene(Scene *scene)
{
	ClearBuffers();
	// Set scene and transformations
	SetScene(scene); 


	// Draw // 
	if (this->show_axes_) { DrawAxes(); }

	if (this->draw_cameras_){// TODO:
	}

	if (this->draw_lights_){// TODO:
		std::vector<Light*> lights = scene->GetLights();
		for (const auto& light : lights) {
			if (light->GetType() == LightType::POINT_LIGHT) {
				MeshModel* model = ((PointLight*)light)->GetLightCubeModel();
				DrawLightCube(model);
			}
		}
	}
	// Draw Models
	std::vector<MeshModel*> mesh_models = scene->GetModels();
	for (const auto& mesh_model : mesh_models) {
		DrawMeshModel(mesh_model, this->draw_wireframe_, true);
	}
}

void Renderer::DrawMeshModel(MeshModel* model, bool is_wireframe, bool draw_normals) {
	if (model == nullptr) { return; }

	// Compute transformations
	model_transform_ = model->GetModelTransform(); // mat4
	mat4 modelview_matrix = view_transform_ * model_transform_;
	// Get vertices 
	matxf vertices_local = model->GetVerticesLocal(); // (4, 3N)
	matxf vertices_camera = modelview_matrix * vertices_local; // (4, 3N)
	// Clipping entire model
	if (is_clipping_) {
		vec3 center_of_mass_camera;
		float radius_bounding_sphere_camera;
		geometry::GetBoundingSphere(vertices_camera, center_of_mass_camera, radius_bounding_sphere_camera);
		if (frustum_clipper_.IsSphereCompletelyOutside(center_of_mass_camera, radius_bounding_sphere_camera)) {
			return;
		}
	}
	// Compute transformations
	mvp_ = projection_transform_ * modelview_matrix;
	// Compute vertices 
	matxf vertices_clip = projection_transform_ * vertices_camera; // (4, 3N)
	matxf vertices_ndc = vertices_clip.array().rowwise() / vertices_clip.row(3).array(); // (4, 3N)
	matxf vertices_raster = (viewport_transform_ * vertices_ndc).topRows(3); // (3, 3N)

	// Get Normals
	if (draw_normals && (this->draw_face_normals_ || this->draw_vertex_normals_)) {
		normal_transform_ = geometry::GetNormalTransfrom(modelview_matrix);
	}
	matxf v_normals_local = model->GetNormalsLocal(); // (3, 3N)
	matxf f_normals_local = model->GetFaceNormalsLocal(); // (3, N)
	matxf v_normals_end_vertices_raster; // vertex normals end points in raster space
	matxf f_normals_end_vertices_raster; // face normals end points in raster space

	if (draw_normals && this->draw_vertex_normals_) {
		matxf v_normals_camera = normal_transform_ * v_normals_local; // (3, 3N)
		matxf v_normals_end_vertices_camera = vertices_camera; // (4, 3N)
		v_normals_end_vertices_camera.topRows(3) += v_normals_camera; // (4, 3N)
		matxf v_normals_end_vertices_clip = projection_transform_ * v_normals_end_vertices_camera; // (4, 3N)
		matxf v_normals_end_vertices_ndc = v_normals_end_vertices_clip.array().rowwise() / v_normals_end_vertices_clip.row(3).array(); // (4, 3N)
		v_normals_end_vertices_raster = (viewport_transform_ * v_normals_end_vertices_ndc).topRows(3); // (3, 3N)
	}
	if (draw_normals && this->draw_face_normals_) {
		matxf faces_midpoints_local = model->GetFacesMidpointsLocal(); // (4, N)
		matxf faces_midpoints_camera = modelview_matrix * faces_midpoints_local; // (4, N)
		matxf f_normals_camera = normal_transform_ * f_normals_local; // (3, N)
		matxf f_normals_end_vertices_camera = faces_midpoints_camera; // (4, N)
		f_normals_end_vertices_camera.topRows(3) += f_normals_camera; // (4, N)

		matxf f_normals_end_vertices_clip = projection_transform_ * f_normals_end_vertices_camera; // (4, N)
		matxf f_normals_end_vertices_ndc = f_normals_end_vertices_clip.array().rowwise() / f_normals_end_vertices_clip.row(3).array(); // (4, N)
		f_normals_end_vertices_raster = (viewport_transform_ * f_normals_end_vertices_ndc).topRows(3); // (3, N)
	}
	matxf vertices_world; 
	matxf v_normals_world;
	matxf f_normals_world;
	if (!is_wireframe) {
		vertices_world = model_transform_ * vertices_local; // (4, 3N)
		mat3 world_normal_transform = geometry::GetWorldNormalTransform(model_transform_);
		v_normals_world = world_normal_transform * v_normals_local; // (3, 3N)
		f_normals_world = world_normal_transform * f_normals_local; // (3, 3N)
	}
	// RenderTriangle For each face
	vec3 forward = scene_->GetActiveCamera()->GetForward();
	for (int i = 0; i < f_normals_local.cols(); i++) {
		// Backface culling
		if (this->is_backface_culling_) {
			vec3 face_normal_local = f_normals_local.col(i);
			float z_dot = forward.dot(face_normal_local);
			if (z_dot >= 0)
				continue;
		}
		// extract v0, v1,v2 from vertices_ndc
		vec3 v0_raster = vertices_raster.col(i * 3);
		vec3 v1_raster = vertices_raster.col(i * 3 + 1);
		vec3 v2_raster = vertices_raster.col(i * 3 + 2);
		// if all vertices share same y value - continue
		if (v0_raster.y() == v1_raster.y() && v0_raster.y() == v2_raster.y()) continue;
		// Compute bounding box
		int min_x, max_x, min_y, max_y;
		GetBoundingBox(v0_raster, v1_raster, v2_raster, min_x, max_x, min_y, max_y);
		// Check if triangle is out of screen
		if (max_x < 0 || min_x > width_ || max_y < 0 || min_y > height_)
			continue;
		// Draw Face normal
		if (draw_normals && this->draw_face_normals_) {
			// Compute midpoint in raster space, using v0_raster, v1_raster, v2_raster
			vec3 face_midpoint_raster = (v0_raster + v1_raster + v2_raster) / 3.0f;
			DrawLine(face_midpoint_raster.x(), face_midpoint_raster.y(), face_midpoint_raster.z(), f_normals_end_vertices_raster.col(i).x(), f_normals_end_vertices_raster.col(i).y(), f_normals_end_vertices_raster.col(i).z(),
				MyRGB((unsigned char)0, (unsigned char)0, (unsigned char)255));
		}
		// Clip bounding box to screen
		uint32_t x0 = std::max(int32_t(0), (int32_t)(std::floor(min_x)));
		uint32_t x1 = std::min(int32_t(width_) - 1, (int32_t)(std::floor(max_x)));
		uint32_t y0 = std::max(int32_t(0), (int32_t)(std::floor(min_y)));
		uint32_t y1 = std::min(int32_t(height_) - 1, (int32_t)(std::floor(max_y)));
		// Draw Vertex normals
		if (draw_normals && this->draw_vertex_normals_) {
			// For each vertex, check if it's in clipped bounding box, and if so, draw its normal
			if (IsInBoundingBox(x0, x1, y0, y1, v0_raster.x(), v0_raster.y())) {
				vec3 v0_normal_end_vertex = v_normals_end_vertices_raster.col(i * 3);
				DrawLine(v0_raster, v0_normal_end_vertex);
			}
			if (IsInBoundingBox(x0, x1, y0, y1, v1_raster.x(), v1_raster.y())) {
				vec3 v1_normal_end_vertex = v_normals_end_vertices_raster.col(i * 3 + 1);
				DrawLine(v1_raster, v1_normal_end_vertex);
			}
			if (IsInBoundingBox(x0, x1, y0, y1, v2_raster.x(), v2_raster.y())) {
				vec3 v2_normal_end_vertex = v_normals_end_vertices_raster.col(i * 3 + 2);
				DrawLine(v2_raster, v2_normal_end_vertex);
			}
		}
		if (is_wireframe) {
			DrawLine(v0_raster, v1_raster);
			DrawLine(v1_raster, v2_raster);
			DrawLine(v2_raster, v0_raster);
			continue;
		}
		// If not wireframe, then rasterize triangle
		// Compute Face Color if needed
		vec3 v0_world = vertices_world.col(i * 3).topRows(3);
		vec3 v1_world = vertices_world.col(i * 3 + 1).topRows(3);
		vec3 v2_world = vertices_world.col(i * 3 + 2).topRows(3);
		vec3 face_normal_world = f_normals_world.col(i);
		vec3 v0_normal_world = v_normals_world.col(i * 3);
		vec3 v1_normal_world = v_normals_world.col(i * 3 + 1);
		vec3 v2_normal_world = v_normals_world.col(i * 3 + 2);
		vec3 color;
		Fragment frag;
		if (this->selected_shading_type == FLAT) {
			frag = Fragment(model->GetMaterial(), v0_world, v1_world, v2_world, v0_normal_world, v1_normal_world, v2_normal_world, face_normal_world, 0,0,0);
			color = frag.ComputeColorFlat(scene_->GetLights(), scene_->GetAmbientLight(), scene_->GetActiveCamera());
		} 
		else if (this->selected_shading_type == WHITE_SHADING) {
			color = WHITE.getAsVec3();
		}
		// compute area with an edge function
		float area = EdgeFunction(v0_raster, v1_raster, v2_raster);
		int dx = x1 - x0;
		for (uint32_t y = y0, index_z_compare = y0 * width_ + x0; y <= y1; ++y, index_z_compare += (width_ - (dx + 1)))
		{
			for (uint32_t x = x0; x <= x1; ++x, index_z_compare++)
			{

				vec3 pixel(x + 0.5, y + 0.5, 0);
				float w0 = EdgeFunction(v1_raster, v2_raster, pixel);
				float w1 = EdgeFunction(v2_raster, v0_raster, pixel);
				float w2 = EdgeFunction(v0_raster, v1_raster, pixel);
				bool is_inside = (w0 >= 0 && w1 >= 0 && w2 >= 0);
				if (area < 0)
					is_inside = (w0 <= 0 && w1 <= 0 && w2 <= 0);
				if (is_inside) {
					w0 /= area;
					w1 /= area;
					w2 /= area;
					float one_over_Z = v0_raster.z() * w0 + v1_raster.z() * w1 + v2_raster.z() * w2;
					float z = 1 / one_over_Z;
					// Depth-buffer test
					if (z <= z_buffer_[index_z_compare]) {
						z_buffer_[index_z_compare] = z;
						if (this->selected_shading_type == FLAT) {
							DrawPixel(x, y, z, color, false);
							continue;
						}
						else if (this->selected_shading_type == WHITE_SHADING) {
							DrawPixel(x, y, z, color, false);
							continue;
						}
						vec3 color;
						frag = Fragment(model->GetMaterial(), v0_world, v1_world, v2_world, v0_normal_world, v1_normal_world, v2_normal_world, face_normal_world, w0, w1, w2);

						switch (this->selected_shading_type) 
						{
						case GOURAUD:
							color = frag.ComputeColorGouraud(scene_->GetLights(), scene_->GetAmbientLight(), scene_->GetActiveCamera());
							break;
						case PHONG:
							color = frag.ComputeColorPhong(scene_->GetLights(), scene_->GetAmbientLight(), scene_->GetActiveCamera());
							break;
						}

						DrawPixel(x, y, z, color, false);
					} // end if depth-buffer test
				} // end if inside
			} // end for x
		} // end for y
	} // end for each Triangle

	// Handle bloom
	if (draw_bloom_)
	{
		vec3 pixel_color;
		float pixel_brightness;

		// apply mask to extract brightness > 1
		for (unsigned int row = 0; row < height_; row++)
		{
			for (unsigned int col = 0; col < width_; col++)
			{
				pixel_brightness = bloom_pre_blur_buffer_[row * width_ + col].dot(GRAYSCALE_VEC3);
				
				if (pixel_brightness <= 1.0) // masking brightness
				{
					bloom_pre_blur_buffer_[row * width_ + col] = vec3::Zero();
				}

			}
		} // bloom_pre_blur_buffer_ should be masked now so everything is black except bright spots

		// apply blur effect on masked buffer
		// 1st: horizontal blur
		// TODO:
		// 2nd: vertical blur
		// TODO:

		// combine bloom_post_blur_buffer_ onto framebuffer_
		// TODO:
	}
}


void Renderer::DrawLightCube(MeshModel* model) {
	if (model == nullptr) { return; }
	mat4 mvp = projection_transform_ * view_transform_ * model->GetModelTransform();

	matxf vertices_clip  = mvp * model->GetVerticesLocal(); // (4, 3N)
	matxf vertices_ndc = vertices_clip.array().rowwise() / vertices_clip.row(3).array(); // (4, 3N)
	matxf vertices_raster = (viewport_transform_ * vertices_ndc).topRows(3); // (3, 3N)
	matxf f_normals_local = model->GetFaceNormalsLocal(); // (3, N)
	// RenderTriangle For each face
	vec3 forward = scene_->GetActiveCamera()->GetForward();
	for (int i = 0; i < f_normals_local.cols(); i++) {
		// Backface culling
		if (this->is_backface_culling_ && forward.dot(f_normals_local.col(i)) >= 0) {
				continue;
		}
		// extract v0, v1,v2 from vertices_ndc
		vec3 v0_raster = vertices_raster.col(i * 3);
		vec3 v1_raster = vertices_raster.col(i * 3 + 1);
		vec3 v2_raster = vertices_raster.col(i * 3 + 2);
		// if all vertices share same y value - continue
		if (v0_raster.y() == v1_raster.y() && v0_raster.y() == v2_raster.y()) continue;
		// Compute bounding box
		int min_x, max_x, min_y, max_y;
		GetBoundingBox(v0_raster, v1_raster, v2_raster, min_x, max_x, min_y, max_y);
		// Check if triangle is out of screen
		if (max_x < 0 || min_x > width_ || max_y < 0 || min_y > height_)
			continue;
		// Clip bounding box to screen
		uint32_t x0 = std::max(int32_t(0), (int32_t)(std::floor(min_x)));
		uint32_t x1 = std::min(int32_t(width_) - 1, (int32_t)(std::floor(max_x)));
		uint32_t y0 = std::max(int32_t(0), (int32_t)(std::floor(min_y)));
		uint32_t y1 = std::min(int32_t(height_) - 1, (int32_t)(std::floor(max_y)));

		MyRGB color_256 = WHITE;
		// compute area with an edge function
		float area = EdgeFunction(v0_raster, v1_raster, v2_raster);
		int dx = x1 - x0;
		for (uint32_t y = y0, index_z_compare = y0 * width_ + x0; y <= y1; ++y, index_z_compare += (width_-(dx+1)))
		{
			for (uint32_t x = x0; x <= x1; ++x, index_z_compare++)
			{
				vec3 pixel(x + 0.5, y + 0.5, 0);
				float w0 = EdgeFunction(v1_raster, v2_raster, pixel);
				float w1 = EdgeFunction(v2_raster, v0_raster, pixel);
				float w2 = EdgeFunction(v0_raster, v1_raster, pixel);
				bool is_inside = (w0 >= 0 && w1 >= 0 && w2 >= 0);
				if (area < 0)
					is_inside = (w0 <= 0 && w1 <= 0 && w2 <= 0);
				if (is_inside) {
					w0 /= area;
					w1 /= area;
					w2 /= area;
					float one_over_Z = v0_raster.z() * w0 + v1_raster.z() * w1 + v2_raster.z() * w2;
					float z = 1 / one_over_Z;
					// Depth-buffer test
					if (z <= z_buffer_[index_z_compare]) {
						z_buffer_[index_z_compare] = z;
						DrawPixel(x, y, z, color_256, false);
					} // end if depth-buffer test
				} // end if inside
			} // end for x
		} // end for y
	} // end for each Triangle
}


void Renderer::DrawLine(const vec3& v0, const vec3& v1, MyRGB color) {
		DrawLine(static_cast<int>(v0.x()), static_cast<int>(v0.y()), v0.z(), static_cast<int>(v1.x()), static_cast<int>(v1.y()), v1.z(), color);
}

void Renderer::DrawLine(int x0, int y0, float z0, int x1, int y1, float z1, MyRGB color) {
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
		std::swap(z0, z1);
	}
	const int dx = x1 - x0;
	const int dy = std::abs(y1 - y0); // Also handle negative slopes
	const float dz = z1 - z0;
	const int ystep = (y0 < y1) ? 1 : -1;
	const float z_step = dz / (float)dx;
	int D = 2 * dy - dx;
	int y = y0;
	float z = z0; // Start z-value
	for (int x = x0; x <= x1; x++) {
		if (steep) {
			DrawPixel(y, x, z, color.getAsVec3());
		}
		else {
			DrawPixel(x, y, z, color.getAsVec3());
		}
		if (D > 0) { // If D > 0, we should move one step in the y direction
			y += ystep;
			D += 2 * (dy - dx);
		}
		else { // Else, don't increment
			D += 2 * dy;
		}
	}
	z += z_step;
}

void Renderer::DrawPixel(int x, int y, float z, vec3 color, bool do_depth_test) {
	if (x < 0 || x >= width_ || y < 0 || y >= height_) {
		return;
	}
	// do z-buffer test
	int index_z_compare = y * width_ + x;
	if (do_depth_test && z > z_buffer_[index_z_compare]) {
		return;
	} 
	else {
		MyRGB color_256 = MyRGB(color);

		z_buffer_[index_z_compare] = z;
		int index_r = INDEX(width_, x, y, 0);
		framebuffer_[index_r] = color_256.r;
		framebuffer_[index_r+1] = color_256.g;
		framebuffer_[index_r+2] = color_256.b;
		bloom_pre_blur_buffer_[y * width_ + x] = color;
		
		return;
	}
}


vec3 Renderer::ProjectPoint(const vec4& v_world) {
	mat4 vp = projection_transform_ * view_transform_;
	vec4 v_clip = vp * v_world;
	vec4 v_ndc = v_clip / v_clip.w();
	vec3 v_raster = (viewport_transform_ * v_ndc).topRows(3);
	return v_raster;
}


void Renderer::Draw_GL_LINE(const vec4& v0_world, const vec4& v1_world, MyRGB color)
{
	vec3 v0_raster = ProjectPoint(v0_world);
	vec3 v1_raster = ProjectPoint(v1_world);
	DrawLine(v0_raster, v1_raster, color);
}

void Renderer::Draw_GL_LINE(const vec3& v0_world, const vec3& v1_world, MyRGB color)
{
	vec4 v0_world_h = v0_world.homogeneous();
	vec4 v1_world_h = v1_world.homogeneous();
	Draw_GL_LINE(v0_world_h, v1_world_h, color);
}

void Renderer::Draw_GL_LINE_STRIP(const matxf& vertices_world, MyRGB color) {
	mat4 vp = projection_transform_ * view_transform_;
	matxf vertices_clip = vp * vertices_world;
	matxf vertices_ndc = vertices_clip.array().rowwise() / vertices_clip.row(3).array();
	matxf vertices_raster = (viewport_transform_ * vertices_ndc).topRows(3);
	for (int i = 0; i < vertices_raster.cols() - 1; i++) {
		DrawLine(vertices_raster.col(i), vertices_raster.col(i + 1), color);
	}
}

void Renderer::Draw_GL_LINE_LOOP(const matxf& vertices_world, MyRGB color)
{ 
	matxf new_points = vertices_world;
	new_points.conservativeResize(4, vertices_world.cols() + 1);
	new_points.col(vertices_world.cols()) = vertices_world.col(0);
	Draw_GL_LINE_STRIP(new_points, color);
}

void Renderer::DrawAxes() {
	// Draw X axis
	Draw_GL_LINE(vec3(0, 0, 0), vec3(1, 0, 0), RED);
	// Draw Y axis
	Draw_GL_LINE(vec3(0, 0, 0), vec3(0, 1, 0), GREEN);
	// Draw Z axis
	Draw_GL_LINE(vec3(0, 0, 0), vec3(0, 0, 1), BLUE);
	
}
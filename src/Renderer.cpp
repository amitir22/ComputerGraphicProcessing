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
	// fill z_buffer with z_far
	std::fill(z_buffer_.get(), z_buffer_.get() + width_ * height_, z_far_);

	model_transform_ = mat4::Identity();
	view_transform_ = mat4::Identity();
	projection_transform_ = mat4::Identity();
	viewport_transform_ = geometry::GetViewportTransform(width_, height_);
	normal_transform_ = mat3::Identity();
	is_perspective_ = false;
	selected_shading_type = FLAT;
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
	view_transform_ = scene->GetActiveCamera()->GetViewTransform();
	projection_transform_ = scene->GetActiveCamera()->GetProjectionTransform();
	is_perspective_ = scene->GetActiveCamera()->IsPerspectiveProjection();

	// TODO get light

	z_near_ = scene->GetActiveCamera()->z_near_;
	z_far_ = scene->GetActiveCamera()->z_far_;
	scene->GetActiveCamera()->GetCanvasShape(canvas_top_, canvas_right_);
}

/////////////////////////////////////////////////////
//				DRAW FUNCTIONS
///////////////////////////////////////////////////

void Renderer::DrawScene(Scene *scene)
{
	ClearBuffers();
	// Set scene and transformations
	SetScene(scene); 
	// Draw models
	std::vector<MeshModel*> models = scene->GetModels();

	if (this->show_cameras_){// TODO:
	}

	if (this->show_lights_){// TODO:
	}

	for (const auto& model : models) {
		// set transforms
		model_transform_ = model->GetModelTransform(); // mat4
		// TODO get model material
		// Compute transformations
		mat4 modelview_matrix = view_transform_ * model_transform_;
		normal_transform_ = geometry::GetNormalTransfrom(modelview_matrix);
		mvp_ = projection_transform_ * modelview_matrix;
		// Get model data
		matxf vertices_local = model->GetVerticesLocal(); // (4, 3N)
		matxf v_normals_local = model->GetNormalsLocal(); // (3, 3N)
		matxf f_normals_local = model->GetFaceNormalsLocal(); // (3, N)
		unsigned int num_faces = f_normals_local.cols(); // N
		// Transform model data
		matxf vertices_world = model_transform_ * vertices_local; // (4, 3N)
		mat3 world_normal_transform = geometry::GetWorldNormalTransform(model_transform_);
		matxf v_normals_world = world_normal_transform * v_normals_local; // (3, 3N)
		matxf f_normals_world = world_normal_transform * f_normals_local; // (3, 3N)
		// camera space
		matxf vertices_camera = modelview_matrix * vertices_local; // (4, 3N)
		matxf v_normals_camera = normal_transform_ * v_normals_local; // (3, 3N)
		// Clipping entire model
		if (is_clipping_) {

		}
		// clip,ndc,raster space
		matxf vertices_clip = projection_transform_ * vertices_camera; // (4, 3N)
		matxf vertices_ndc = vertices_clip.array().rowwise() / vertices_clip.row(3).array(); // (4, 3N)
		matxf vertices_raster = (viewport_transform_ * vertices_ndc).topRows(3); // (3, 3N)
		
		if (this->show_vertex_normals_) {
			matxf v_normals_end_vertices_camera = vertices_camera; // (4, 3N)
			v_normals_end_vertices_camera.topRows(3) += v_normals_camera; // (4, 3N)
			matxf v_normals_end_vertices_clip = projection_transform_ * v_normals_end_vertices_camera; // (4, 3N)
			matxf v_normals_end_vertices_ndc = v_normals_end_vertices_clip.array().rowwise() / v_normals_end_vertices_clip.row(3).array(); // (4, 3N)
			v_normals_end_vertices_raster = (viewport_transform_ * v_normals_end_vertices_ndc).topRows(3); // (3, 3N)
		}
		if (this->show_face_normals_) {
			matxf faces_midpoints_local = model->GetFacesMidpointsLocal(); // (4, N)
			matxf faces_midpoints_camera = modelview_matrix * faces_midpoints_local; // (4, N)
			matxf f_normals_camera = normal_transform_ * f_normals_local; // (3, N)
			matxf f_normals_end_vertices_camera = faces_midpoints_camera; // (4, N)
			f_normals_end_vertices_camera.topRows(3) += f_normals_camera; // (4, N)

			matxf f_normals_end_vertices_clip = projection_transform_ * f_normals_end_vertices_camera; // (4, N)
			matxf f_normals_end_vertices_ndc = f_normals_end_vertices_clip.array().rowwise() / f_normals_end_vertices_clip.row(3).array(); // (4, N)
			f_normals_end_vertices_raster = (viewport_transform_ * f_normals_end_vertices_ndc).topRows(3); // (3, N)
		}
		// RenderTriangle For each face
		vec3 forward = scene_->GetActiveCamera()->GetForward();
		for (int i = 0; i < num_faces; i++)
		{
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
			if (this->show_face_normals_) {
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
			if (this->show_vertex_normals_) {
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
			if (this->show_wireframe_) {
				DrawLine(v0_raster, v1_raster);
				DrawLine(v1_raster, v2_raster);
				DrawLine(v2_raster, v0_raster);
				continue;
			}
			// If not wireframe, then rasterize triangle
			// compute area with an edge function
			float area = EdgeFunction(v0_raster, v1_raster, v2_raster);
			for (uint32_t y = y0; y <= y1; ++y)
			{
				for (uint32_t x = x0; x <= x1; ++x) {
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
						if (z <= z_buffer_[y * width_ + x]) {
							z_buffer_[y * width_ + x] = z;
							// TODO fragment shader - compute color, according to selected lighting method
							// TODO: create fragment and pass color
							// Get Face Normal world
							vec3 v0_world = vertices_world.col(i * 3).topRows(3);
							vec3 v1_world = vertices_world.col(i * 3 + 1).topRows(3);
							vec3 v2_world = vertices_world.col(i * 3 + 2).topRows(3);
							vec3 face_normal_world = f_normals_world.col(i);
							vec3 v0_normal_world = v_normals_world.col(i * 3);
							vec3 v1_normal_world = v_normals_world.col(i * 3 + 1);
							vec3 v2_normal_world = v_normals_world.col(i * 3 + 2);

							Fragment frag = Fragment(model->GetMaterial(), v0_world, v1_world, v2_world, v0_normal_world, v1_normal_world, v2_normal_world, face_normal_world, w0, w1, w2);

							vec3 color;
							switch (this->selected_shading_type)
							{
							case FLAT:
								color = frag.ComputeColorFlat(scene_->GetLights(), scene_->GetAmbientLight(), scene_->GetActiveCamera());
								break;
							case GOURAUD:
								color = frag.ComputeColorGouraud(scene_->GetLights(), scene_->GetAmbientLight(), scene_->GetActiveCamera());
								break;
							case PHONG:
								color = frag.ComputeColorPhong(scene_->GetLights(), scene_->GetAmbientLight(), scene_->GetActiveCamera());
								break;
							}
							MyRGB color256 = MyRGB(color);

							DrawPixel(x, y, z, color256);
						} // end if depth-buffer test
					} // end if inside
				} // end for x
			} // end for y
			
		} // end for each Triangle
	} // end for each model
}

void Renderer::DrawLine(const vec3& v0, const vec3& v1, MyRGB color) {
		DrawLine(static_cast<int>(v0.x()), static_cast<int>(v0.y()), v0.z(), static_cast<int>(v1.x()), static_cast<int>(v1.y()), v1.z(), color);
}
void Renderer::DrawLine(const vec3& v0, const vec3& v1) {
	DrawLine(v0, v1, MyRGB((unsigned char)255, (unsigned char)255, (unsigned char)255));
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
			DrawPixel(y, x, z, color);
		}
		else {
			DrawPixel(x, y, z, color);
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

void Renderer::DrawLine(int x0, int y0, float z0, int x1, int y1, float z1)
{
	DrawLine(x0, y0, z0, x1, y1, z1, MyRGB((unsigned char)255, (unsigned char)255, (unsigned char)255));
}


void Renderer::DrawPixel(int x, int y, float z, MyRGB color) {
	if (x < 0 || x >= width_ || y < 0 || y >= height_) {
		return;
	}
	// do z-buffer test
	if (z <= z_buffer_[y * width_ + x]) {
		z_buffer_[y * width_ + x] = z;
		framebuffer_[INDEX(width_, x, y, 0)] = color.r;
		framebuffer_[INDEX(width_, x, y, 1)] = color.g;
		framebuffer_[INDEX(width_, x, y, 2)] = color.b;
	}
}

void Renderer::DrawPixel(int x, int y, float z) {
	DrawPixel(x, y, z, MyRGB((unsigned char)255, (unsigned char)255, (unsigned char)255));
}

void Renderer::DrawPixel(const vec3& v) {
	DrawPixel(static_cast<int>(v.x()), static_cast<int>(v.y()), v.z());
}


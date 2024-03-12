// MeshModel.h
#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "Geometry.h"
#include "Material.h"
#include "MeshUtils.h"
#include "Camera.h"


using namespace std;


class MeshModel
{
public:
	shared_ptr<Camera> personal_camera;
	string model_name_;
	std::unique_ptr<Material> material_;
	matxf vertices_local_;
	matxf v_normals_local_; // Normalized
	matxf face_normals_local_; // Normalized
	matxf faces_midpoints_local_;
	// Bounding Box
	geometry::Box bounding_box_;
	vec3 center_of_mass_;
	float radius_bounding_sphere_; // radius from center of mass to the farthest vertex 

	mat4 model_transform_; // also known as model transform
	vec3 model_translation_ = vec3(0, 0, 0);
	vec3 model_pitch_yaw_roll_ = vec3(0, 0, 0);
	vec3 model_scale_ = vec3(1,1,1);

	MeshModel() noexcept;

	explicit MeshModel(string fileName);
	void SetModelName(string file_name);
	std::string GetModelName() const { return model_name_; }
	void LoadFile(string fileName);

	// Model Transformations
	void Translate(vec3 translation);
	void SetTranslation(vec3 translation);
	void UpdateTransformation();
	void Rotate(vec3 pitch_yaw_roll);
	void Rotate(float pitch, float yaw, float roll){ this->Rotate(vec3(pitch, yaw, roll)); }
	void Rotate(vec3 axis, float angle);
	void SetScale(vec3 scale);
	void SetRotation(vec3 pitch_yaw_roll);
	mat4 GetModelTransform() const {return model_transform_;}
	// Model Data
		// Vertices
	const matxf& GetVerticesLocal() const { return vertices_local_; }
	const matxf& GetNormalsLocal() const { return v_normals_local_; } // Normalized
	const matxf& GetFaceNormalsLocal() const { return face_normals_local_; } // Normalized
	const matxf& GetFacesMidpointsLocal() const {return faces_midpoints_local_;}
		// Material
	void SetMaterial(std::unique_ptr<Material> newMaterial) { material_ = std::move(newMaterial);};
	Material* GetMaterial() {return material_.get();}
	Camera* GetModelCamera() { return this->personal_camera.get(); }

		// Translation, Rotation, Scale
	vec3 GetTranslation() const { return model_translation_; }
	vec3 GetRotation() const { return model_pitch_yaw_roll_;}
	vec3 GetScale() const { return model_scale_; }
};

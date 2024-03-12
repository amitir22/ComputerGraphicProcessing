// MeshModel.cpp
#include "MeshModel.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <Eigen/Dense>

#include "Geometry.h" // For vec3fFromStream

MeshModel::MeshModel() noexcept
{
	model_transform_ = mat4::Identity();
	material_ = std::make_unique<UniformMaterial>(vec3::Random().cwiseMax(0).cwiseMin(1));
}

MeshModel::MeshModel(string file_name) : MeshModel()
{
	LoadFile(file_name);
	
	// TODO: check this:
	vec3 camera_location = 1.1 * radius_bounding_sphere_ * vec3(0, 0, -1) + center_of_mass_;
	personal_camera = make_shared<Camera>(camera_location, center_of_mass_);
}

void MeshModel::SetModelName(string file_name) {
	// Extract the file name (removing directories and .obj extension) from the path.
	size_t last_slash_idx = file_name.find_last_of("\\/");
	if (std::string::npos != last_slash_idx)
	{
		model_name_ = file_name.substr(last_slash_idx + 1, file_name.size() - 5 - last_slash_idx);
	}
	else
	{
		model_name_ = file_name.substr(0, file_name.size() - 4);
	}
}


// Read OBJ file and create a mesh model
void MeshModel::LoadFile(string file_name)
{
	SetModelName(file_name);
	// Open file
	ifstream ifile(file_name.c_str());
	vector<vec3> vertices;
	vector<vec3> normals;
	vector<FaceIdcs> faces_indices;
	// while not end of file
	while (!ifile.eof())
	{
		// get line
		string curLine;
		getline(ifile, curLine);

		// read type of the line
		istringstream issLine(curLine);
		string lineType;

		issLine >> std::ws >> lineType;

		if (lineType == "v")
			vertices.push_back(vec3fFromStream(issLine));
		else if (lineType == "vn")
			normals.push_back(vec3fFromStream(issLine));
		else if (lineType == "f")
			faces_indices.push_back(FaceIdcs(issLine));
		else if (lineType == "#" || lineType == "") {
			// do nothing if the line is a comment or empty
		}
		else {
			cout << "Found unknown line Type \"" << lineType << "\"";
		}
	}
	ifile.close();
	unsigned int num_of_faces = faces_indices.size();
	 // Initialize Mesh Data
	vertices_local_ = Eigen::MatrixXf(4, num_of_faces*3);
	Eigen::MatrixXf temp_v_normals = Eigen::MatrixXf::Zero(3, vertices.size());
	face_normals_local_ = Eigen::MatrixXf(3, num_of_faces);
	faces_midpoints_local_ = Eigen::MatrixXf(4, num_of_faces);
	
	for (int i = 0; i < num_of_faces; i++) {
		unsigned short v0_index = faces_indices[i].v[0] - 1;
		unsigned short v1_index = faces_indices[i].v[1] - 1;
		unsigned short v2_index = faces_indices[i].v[2] - 1;
		vec3 v0 = vertices[v0_index];
		vec3 v1 = vertices[v1_index];
		vec3 v2 = vertices[v2_index];
		// set columns i, i+1, i+2 of vertices_local_ to a homogenized v0, v1, v2
		vertices_local_.col(i*3)     << v0, 1;
		vertices_local_.col((i*3)+1) << v1, 1;
		vertices_local_.col((i*3)+2) << v2, 1;
		// compute face normal
		vec3 n = (v1 - v0).cross(v2 - v0);
		// check if normals is empty
		if (normals.size() == 0)
		{
			temp_v_normals.col(v0_index) += n;	
			temp_v_normals.col(v1_index) += n;
			temp_v_normals.col(v2_index) += n;
		}
		else {
			temp_v_normals.col(v0_index) += normals[faces_indices[i].vn[0] - 1];
			temp_v_normals.col(v1_index) += normals[faces_indices[i].vn[1] - 1];
			temp_v_normals.col(v2_index) += normals[faces_indices[i].vn[2] - 1];
		}
		n.normalize();
		// set column i of face_normals_local_ to n
		face_normals_local_.col(i) << n;
		// compute face midpoint
		vec3 face_midpoint = (v0 + v1 + v2) / 3.0f;
		faces_midpoints_local_.col(i) << face_midpoint, 1;
	} // end for loop of faces
	// Normalize temp_v_normals, columnwise 
	temp_v_normals.colwise().normalize();
	// compute vertex normals
	v_normals_local_ = Eigen::MatrixXf(3, num_of_faces * 3);
	for (int i = 0; i < num_of_faces; i++) {
		unsigned short v0_index = faces_indices[i].v[0] - 1;
		unsigned short v1_index = faces_indices[i].v[1] - 1;
		unsigned short v2_index = faces_indices[i].v[2] - 1;
		v_normals_local_.col(i * 3) << temp_v_normals.col(v0_index);
		v_normals_local_.col((i * 3) + 1) << temp_v_normals.col(v1_index);
		v_normals_local_.col((i * 3) + 2) << temp_v_normals.col(v2_index);
	}
	// Compute the bounding box and sphere of the model
	bounding_box_ = geometry::GetBoundingBox(vertices_local_);
	center_of_mass_ = geometry::GetCenterOfMass(vertices_local_);
	radius_bounding_sphere_ = geometry::GetRadiusBoundingSphere(vertices_local_, center_of_mass_);
}

////////////////////////////////////////
// 		Model Transformations	  //
////////////////////////////////////////
void MeshModel::UpdateTransformation()
{
	mat4 translation_matrix = geometry::GetTranslationMatrix(model_translation_);
	mat4 rotation_matrix = geometry::GetRotationMatrix(model_pitch_yaw_roll_.x(), model_pitch_yaw_roll_.y(), model_pitch_yaw_roll_.z());
	mat4 scale_matrix = geometry::GetScaleMatrix(model_scale_);
	model_transform_ = translation_matrix * rotation_matrix * scale_matrix;
}

void MeshModel::Translate(vec3 translation)
{
	model_translation_ += translation;
	model_transform_ = geometry::GetTranslationMatrix(translation) * model_transform_;
}

void MeshModel::SetTranslation(vec3 translation)
{
	model_translation_ = translation;
	UpdateTransformation();
}

void MeshModel::Rotate(vec3 pitch_yaw_roll)
{
	model_pitch_yaw_roll_ += pitch_yaw_roll;
	model_transform_ = geometry::GetRotationMatrix(pitch_yaw_roll.x(), pitch_yaw_roll.y(), pitch_yaw_roll.z()) * model_transform_;
}

void MeshModel::SetRotation(vec3 pitch_yaw_roll)
{
	model_pitch_yaw_roll_ = pitch_yaw_roll;
	UpdateTransformation();
}

void MeshModel::Rotate(vec3 axis, float angle) {
		model_transform_ = geometry::GetRotationMatrix(axis, angle) * model_transform_;
}

void MeshModel::SetScale(vec3 scale)
{
	model_scale_ = scale;
	UpdateTransformation();
}


MeshModel::MeshModel(vec3 cube_center)
{
	vertices_local_.resize(4,36);
	vertices_local_ << 
		-0.5, 0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, 0.5, 0.5, 0.5, -0.5, -0.5,
		-0.5, -0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
		0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, 0.5,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1;

	v_normals_local_.resize(3,36);

	v_normals_local_ << 
		-0.408248, 0.666667, 0.408248, 0.408248, -0.666667, -0.408248, 0.408248, -0.666667, -0.408248, -0.408248, 0.666667, 0.408248, -0.666667, -0.408248, -0.666667, -0.666667, -0.408248, -0.666667, 0.666667, 0.408248, 0.666667, 0.666667, 0.408248, 0.666667, -0.666667, 0.408248, 0.666667, 0.666667, -0.408248, -0.666667, -0.666667, 0.408248, 0.666667, 0.666667, -0.408248, -0.666667,
		-0.408248, -0.666667, 0.408248, 0.408248, 0.666667, -0.408248, -0.408248, -0.666667, 0.408248, 0.408248, 0.666667, -0.408248, -0.666667, -0.408248, 0.666667, 0.666667, 0.408248, -0.666667, -0.666667, -0.408248, 0.666667, 0.666667, 0.408248, -0.666667, -0.666667, -0.408248, -0.666667, -0.666667, -0.408248, -0.666667, 0.666667, 0.408248, 0.666667, 0.666667, 0.408248, 0.666667,
		0.816497, 0.333333, 0.816497, 0.816497, 0.333333, 0.816497, -0.816497, -0.333333, -0.816497, -0.816497, -0.333333, -0.816497, -0.333333, 0.816497, 0.333333, 0.333333, -0.816497, -0.333333, 0.333333, -0.816497, -0.333333, -0.333333, 0.816497, 0.333333, -0.333333, -0.816497, 0.333333, 0.333333, 0.816497, -0.333333, 0.333333, 0.816497, -0.333333, -0.333333, -0.816497, 0.333333;

	face_normals_local_.resize(3, 12);
	face_normals_local_ << 
		0, 0, 0, 0, -1, -1, 1, 1, 0, -0, -0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 1, 1,
		1, 1, -1, -1, 0, -0, 0, -0, 0, 0, 0, 0;
	
	
	faces_midpoints_local_.resize(4, 12);
	faces_midpoints_local_ << 
		0.166667, -0.166667, -0.166667, 0.166667, -0.5, -0.5, 0.5, 0.5, 0.166667, -0.166667, 0.166667, -0.166667,
		-0.166667, 0.166667, -0.166667, 0.166667, -0.166667, 0.166667, -0.166667, 0.166667, -0.5, -0.5, 0.5, 0.5,
		0.5, 0.5, -0.5, -0.5, 0.166667, -0.166667, -0.166667, 0.166667, -0.166667, 0.166667, 0.166667, -0.166667,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1;

	center_of_mass_ = vec3(0, 0, 0);
	radius_bounding_sphere_ = 0.866025388f;

	this->SetTranslation(cube_center);
}

void printMatrixForInit(const Eigen::MatrixXf& mat) {
	for (int i = 0; i < mat.rows(); ++i) {
		for (int j = 0; j < mat.cols(); ++j) {
			std::cout << mat(i, j);
			if (j < mat.cols() - 1) std::cout << ", "; 
		}
		std::cout << ",\n"; 
	}
}

void PrintMeshModel(const MeshModel& model) {
	std::cout << "vertices_local_" << "\n";
	printMatrixForInit(model.vertices_local_);
	std::cout << "\n";

	std::cout << "v_normals_local_" << "\n";
	printMatrixForInit(model.v_normals_local_);
	std::cout << "\n";

	std::cout << "face_normals_local_" << "\n";
	printMatrixForInit(model.face_normals_local_);
	std::cout << "\n";

	std::cout << "faces_midpoints_local_" << "\n";
	printMatrixForInit(model.faces_midpoints_local_);
	std::cout << "\n";

	std::cout << "center_of_mass_" << "\n";
	std::cout << model.center_of_mass_ << "\n";


	std::cout << "radius_bounding_sphere_" << "\n";
	std::cout << model.radius_bounding_sphere_ << "\n";

}
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
	material = UniformMaterial(Eigen::Vector3f::Random());
}

MeshModel::MeshModel(string file_name) : MeshModel()
{
	LoadFile(file_name);
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
	std::cout << "Loading model " << model_name_ << " from file " << file_name << std::endl;

	model_name_ = file_name;
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
	// Compute the bounding box of the model
	ComputeBoundingBox();
}

void MeshModel::ComputeBoundingBox()
{
	// Compute the bounding box of the model
	vec3 min = vertices_local_.block(0, 0, 3, vertices_local_.cols()).rowwise().minCoeff();
	vec3 max = vertices_local_.block(0, 0, 3, vertices_local_.cols()).rowwise().maxCoeff();
	bounding_box_ = geometry::Box(min, max);
}


////////////////////////////////////////
// 		Transformation functions	  //
////////////////////////////////////////
void MeshModel::Translate(vec3 translation)
{
	model_transform_ = geometry::GetTranslationMatrix(translation) * model_transform_;
}

void MeshModel::Rotate(vec3 axis, float angle)
{
	model_transform_ = geometry::GetRotationMatrix(axis, angle) * model_transform_;
}

void MeshModel::Scale(vec3 scale)
{
	model_transform_ = geometry::GetScaleMatrix(scale) * model_transform_;
}

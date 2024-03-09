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
}

MeshModel::MeshModel(string file_name) : MeshModel()
{
	LoadFile(file_name);
}

// Read OBJ file and create a mesh model
void MeshModel::LoadFile(string file_name)
{
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
	 //Initialize vertices_local_, normals_local_, and face_normals_local_
	vertices_local_ = Eigen::MatrixXf(4, faces_indices.size()*3);
	normals_local_ = Eigen::MatrixXf(3, faces_indices.size()*3);
	face_normals_local_ = Eigen::MatrixXf(3, faces_indices.size());
	faces_midpoints_local_ = Eigen::MatrixXf(4, faces_indices.size());
	
	for (int i = 0; i < faces_indices.size(); i++) {
		vec3 v0,v1,v2;
		v0 = vertices[faces_indices[i].v[0] - 1];
		v1 = vertices[faces_indices[i].v[1] - 1];
		v2 = vertices[faces_indices[i].v[2] - 1];
		// set columns i, i+1, i+2 of vertices_local_ to a homogenized v0, v1, v2
		vertices_local_.col(i*3)     << v0, 1;
		vertices_local_.col((i*3)+1) << v1, 1;
		vertices_local_.col((i*3)+2) << v2, 1;
		// set columns i, i+1, i+2 of normals_local_ to n0, n1, n2
		normals_local_.col(i*3)     << normals[faces_indices[i].vn[0] - 1];
		normals_local_.col((i*3)+1) << normals[faces_indices[i].vn[1] - 1];
		normals_local_.col((i*3)+2) << normals[faces_indices[i].vn[2] - 1];
		// compute face normal
		vec3 n = (v1 - v0).cross(v2 - v0);
		n.normalize();
		// set column i of face_normals_local_ to n
		face_normals_local_.col(i) << n;
		// compute face midpoint, set column i of faces_midpoints_local_ to a homogenized midpoint
		vec3 midpoint = (v0 + v1 + v2) / 3;
		faces_midpoints_local_.col(i) << midpoint, 1;
	}		
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

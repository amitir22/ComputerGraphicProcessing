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
	vector<FaceIdcs> faces_indices;
	vector<vec3> vertices;
	vector<vec3> normals;
	// while not end of file
	while (!ifile.eof()) {
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
		//Vertex_positions is an array of vec3. Every three elements define a triangle in 3D.
		//If the face part of the obj is
		//f 1 2 3
		//f 1 3 4
		//Then vertex_positions should contain:
		//vertex_positions={v1,v2,v3,v1,v3,v4}
		//vertex_positions.resize(faces_indices.size() * 3);
		//normal_positions.resize(faces_indices.size() * 3);
		for (int i = 0; i < faces_indices.size(); i++) {
			Vertex v1, v2, v3;

			v1.position_in_local_space = vertices[faces_indices[i].v[0] - 1];
			v1.normal_in_local_space = normals[faces_indices[i].vn[0] - 1];
			v2.position_in_local_space = vertices[faces_indices[i].v[1] - 1];
			v2.normal_in_local_space = normals[faces_indices[i].vn[1] - 1];
			v3.position_in_local_space = vertices[faces_indices[i].v[2] - 1];
			v3.normal_in_local_space = normals[faces_indices[i].vn[2] - 1];
			//vertex_positions[i * 3 + j] = vertices[faces_indices[i].v[j] - 1];
			//normal_positions[i * 3 + j] = normals[faces_indices[i].vn[j] - 1];

			Face face = Face(v1, v2, v3);

			faces.push_back(face);
		}
	}
}

////////////////////////////////////////
// 		Transformation functions	  //
////////////////////////////////////////
void MeshModel::Translate(vec3 translation)
{
	model_transform_ = geometry::makeTranslationMatrix(translation) * model_transform_;
}

void MeshModel::Rotate(vec3 axis, float angle)
{
	model_transform_ = geometry::makeRotationMatrix(axis, angle) * model_transform_;
}

void MeshModel::Scale(vec3 scale)
{
	model_transform_ = geometry::makeScaleMatrix(scale) * model_transform_;
}

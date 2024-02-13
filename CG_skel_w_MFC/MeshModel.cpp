// MeshModel.cpp
#include "StdAfx.h"
#include "MeshModel.h"
#include "vec.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
vec3 vec3fFromStream(std::istream& aStream)
{
	float x, y, z;
	aStream >> x >> std::ws >> y >> std::ws >> z;
	return vec3(x, y, z);
}

vec2 vec2fFromStream(std::istream& aStream)
{
	float x, y;
	aStream >> x >> std::ws >> y;
	return vec2(x, y);
}


MeshModel::MeshModel() noexcept
{
	_model_transform = mat4(1.0f);
	_normal_transform = mat3(1.0f);
}


MeshModel::MeshModel(string fileName) : MeshModel()
{	
	loadFile(fileName);
}

// Read OBJ file and create a mesh model
void MeshModel::loadFile(string fileName)
{
	ifstream ifile(fileName.c_str());
	vector<FaceIdcs> faces;
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
			faces.push_back(FaceIdcs(issLine));
		else if (lineType == "#" || lineType == "") { 
			// do nothing if the line is a comment or empty
		}
		else {
			cout<< "Found unknown line Type \"" << lineType << "\"";
		}
		//Vertex_positions is an array of vec3. Every three elements define a triangle in 3D.
		//If the face part of the obj is
		//f 1 2 3
		//f 1 3 4
		//Then vertex_positions should contain:
		//vertex_positions={v1,v2,v3,v1,v3,v4}
		vertex_positions.resize(faces.size() * 3);
		normal_positions.resize(faces.size() * 3);
		for (int i = 0; i < faces.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				vertex_positions[i * 3 + j] = vertices[faces[i].v[j] - 1];
				normal_positions[i * 3 + j] = normals[faces[i].vn[j] - 1];
			}
		}
	}
}



void MeshModel::draw(Renderer& renderer)
{
	renderer.SetModelMatrices(_model_transform, _normal_transform);
	renderer.DrawTriangles(&vertex_positions, &normal_positions);
}

////////////////////////////////////////
// 		Transformation functions	  //
////////////////////////////////////////
void MeshModel::translate(vec3 translation)
{ // TODO
}

void MeshModel::rotate(vec3 axis, float angle)
{ // TODO
}

void MeshModel::scale(vec3 scale)
{ // TODO
}

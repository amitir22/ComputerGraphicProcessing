// MeshModel.cpp
#include "StdAfx.h"
#include "MeshModel.h"

static inline face* makeTriangleFacesForBox(vec3 min, vec3 max);

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



void MeshModel::draw(Renderer& renderer, bool isDrawNormals)
{
	renderer.SetModelMatrices(_model_transform, _normal_transform);
	renderer.updateMatrices();
	renderer.DrawTriangles(&vertex_positions, &normal_positions, isDrawNormals);
}

void MeshModel::applyTransformation(mat4 transformation)
{
	_model_transform = transformation * _model_transform;
	//_normal_transform = transformation * _normal_transform; // TODO apply
}

////////////////////////////////////////
// 		Transformation functions	  //
////////////////////////////////////////

void MeshModel::translate(vec3 translation)
{ 
	_model_transform = Geometry::makeTranslationMatrix(translation) * _model_transform;
}

void MeshModel::rotate(vec3 axis, float angle)
{ 
	_model_transform = Geometry::makeRotationMatrix(axis, angle) * _model_transform;
	
}

void MeshModel::scale(vec3 scale)
{ 
	_model_transform = Geometry::makeScaleMatrix(scale) * _model_transform;
}

void MeshModel::drawBoundingBox(Renderer& renderer)
{
	vec3 min;
	vec3 max;

	// calc min and max
	// TODO: amit

	BoxMeshModel boundingBox = BoxMeshModel::makeBoundingBox(0.3, 0.6, 0.9, min, max);
	boundingBox.draw(renderer);
}

void MeshModel::drawVertexNormals(Renderer& renderer)
{
	// TODO: ehud
}

void MeshModel::drawFaceNormals(Renderer& renderer)
{
	// TODO: ehud
}

vec3 MeshModel::getNormalOfFace(face f)
{
	vec3 v1, v2;

	v1 = f.v[1] - f.v[0];
	v2 = f.v[2] - f.v[0];

	vec3 normal = cross(v1, v2);

	return normal;
}

CubeMeshModel::CubeMeshModel() noexcept : MeshModel()
{
	vec3 min = CUBE_MESH_DEFAULT_MIN;
	vec3 max = CUBE_MESH_DEFAULT_MAX;

	this->vertex_positions = std::vector<vec3>();

	face* f = makeTriangleFacesForBox(min, max);

	for (int faceIndex = 0; faceIndex < 12; ++faceIndex) // 12 traingles make the 6 faces of a box
	{
		this->vertex_positions.push_back(f[faceIndex].v[0]);
		this->vertex_positions.push_back(f[faceIndex].v[1]);
		this->vertex_positions.push_back(f[faceIndex].v[2]);
	}

	for (int vertexIndex = 0; vertexIndex < 8; ++vertexIndex) // 8 vertices for the box
	{
		this->normal_positions.push_back(vec3(0, 0, 0)); // 0,0,0 = white
	}
}

BoxMeshModel::BoxMeshModel() : BoxMeshModel(CUBE_MESH_DEFAULT_MIN, CUBE_MESH_DEFAULT_MAX) {}

BoxMeshModel::BoxMeshModel(vec3 min, vec3 max) : MeshModel()
{
	this->vertex_positions = std::vector<vec3>();

	face* f = makeTriangleFacesForBox(min, max);

	for (int faceIndex = 0; faceIndex < 12; ++faceIndex) // 12 traingles make the 6 faces of a box
	{
		this->vertex_positions.push_back(f[faceIndex].v[0]);
		this->vertex_positions.push_back(f[faceIndex].v[1]);
		this->vertex_positions.push_back(f[faceIndex].v[2]);
	}

	for (int vertexIndex = 0; vertexIndex < 8; ++vertexIndex) // 8 vertices for the box
	{
		this->normal_positions.push_back(vec3(0, 0, 0)); // 0,0,0 = white
	}
}

BoxMeshModel BoxMeshModel::makeBoundingBox(float red, float green, float blue,
	vec3 min, vec3 max)
{
	BoxMeshModel boundingBox = BoxMeshModel(min, max);

	for (int vertexIndex = 0; vertexIndex < 8; ++vertexIndex)
	{
		boundingBox.normal_positions[vertexIndex] = vec3(red, green, blue);
	}

	return boundingBox;
}

LineModel::LineModel() noexcept
{
	this->startPoint = vec3(-1, -1, -1);
	this->endPoint = vec3(-1, -1, -1);
	this->color = vec3(0, 0, 0); // black
}

LineModel::LineModel(vec3 startPoint, vec3 endPoint, vec3 color)
	: startPoint(startPoint), endPoint(endPoint), color(color) { }

void LineModel::draw(Renderer& renderer)
{
	// TODO
}


// Local inline functions

static inline face* makeTriangleFacesForBox(vec3 min, vec3 max)
{
	face f[12];
	vec3 v0 = min;
	vec3 v1 = vec3(min.x, min.y, max.z);
	vec3 v2 = vec3(min.x, max.y, min.z);
	vec3 v3 = vec3(max.x, min.y, min.z);
	vec3 v4 = vec3(min.x, max.y, max.z);
	vec3 v5 = vec3(max.x, max.y, min.z);
	vec3 v6 = vec3(max.x, min.y, max.z);
	vec3 v7 = max;

	f[0] = { v0, v1, v2 };
	f[1] = { v1, v4, v2 };
	f[2] = { v0, v2, v3 };
	f[3] = { v2, v5, v3 };
	f[4] = { v0, v3, v6 };
	f[5] = { v0, v6, v1 };
	f[6] = { v3, v5, v7 };
	f[7] = { v3, v7, v6 };
	f[8] = { v1, v7, v4 };
	f[9] = { v1, v6, v7 };
	f[10] = { v2, v7, v5 };
	f[11] = { v2, v4, v7 };

	return f;
}
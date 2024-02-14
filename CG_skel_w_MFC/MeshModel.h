// MeshModel.h
#pragma once
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include "vec.h"
#include "mat.h"
#include "Renderer.h"
#include "Geometry.h"

typedef Geometry::Face face;

#define CUBE_DEFAULT_SIZE 2
#define CUBE_MESH_DEFAULT_MIN (vec3(-0.5 * CUBE_DEFAULT_SIZE, -0.5 * CUBE_DEFAULT_SIZE, -0.5 * CUBE_DEFAULT_SIZE))
#define CUBE_MESH_DEFAULT_MAX (vec3(0.5 * CUBE_DEFAULT_SIZE, 0.5 * CUBE_DEFAULT_SIZE, 0.5 * CUBE_DEFAULT_SIZE))

using namespace std;

struct FaceIdcs
{
	int v[4];
	int vn[4];
	int vt[4];

	FaceIdcs()
	{
		for (int i = 0; i < 4; i++)
			v[i] = vn[i] = vt[i] = 0;
	}

	FaceIdcs(std::istream& aStream)
	{
		for (int i = 0; i < 4; i++)
			v[i] = vn[i] = vt[i] = 0;

		char c;
		for (int i = 0; i < 3; i++)
		{
			aStream >> std::ws >> v[i] >> std::ws;
			if (aStream.peek() != '/')
				continue;
			aStream >> c >> std::ws;
			if (aStream.peek() == '/')
			{
				aStream >> c >> std::ws >> vn[i];
				continue;
			}
			else
				aStream >> vt[i];
			if (aStream.peek() != '/')
				continue;
			aStream >> c >> vn[i];
		}
	}
};

class MeshModel
{
public:
	vec3 vMin;
	vec3 vMax;
	vec3 vMiddle;
	std::vector<vec3> vertex_positions;
	std::vector<vec3> normal_positions;

	mat4 _model_transform; // also known as model transform
	mat3 _normal_transform;

	MeshModel() noexcept;
	explicit MeshModel(string fileName);
	void loadFile(string fileName);
	void draw(Renderer& renderer);
	void applyTransformation(mat4 transformation);
	// User Transformation
	void translate(vec3 translation);
	void rotate(vec3 axis, float angle);
	void scale(vec3 scale);
	// MeshModel options
	void drawBoundingBox(Renderer& renderer);
	void drawVertexNormals(Renderer& renderer);
	void drawFaceNormals(Renderer& renderer);

	static vec3 getNormalOfFace(face f);
};

class CubeMeshModel : public MeshModel 
{
public:
	CubeMeshModel() noexcept;
};

class BoxMeshModel : public MeshModel
{
public:
	BoxMeshModel();
	BoxMeshModel(vec3 min, vec3 max);
	static BoxMeshModel makeBoundingBox(float red, float green, float blue,
		vec3 min, vec3 max);
};

class LineModel
{
public:
	vec3 startPoint;
	vec3 endPoint;
	vec3 color;

	LineModel() noexcept;
	LineModel(vec3 startPoint, vec3 endPoint, vec3 color);
	void draw(Renderer& renderer);
};
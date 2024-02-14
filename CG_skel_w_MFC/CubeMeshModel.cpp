#include "stdafx.h"
#include "CubeMeshModel.h"

typedef Geometry::Face face;

static inline face* makeTriangleFacesForBox(vec3 min, vec3 max);

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

	face *f = makeTriangleFacesForBox(min, max);

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
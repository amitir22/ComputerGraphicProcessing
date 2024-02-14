#pragma once
#include <memory>
#include "MeshModel.h"
#include "Geometry.h"

#define CUBE_DEFAULT_SIZE 20
#define CUBE_MESH_DEFAULT_MIN (vec3(0, 0, 0))
#define CUBE_MESH_DEFAULT_MAX (vec3(CUBE_DEFAULT_SIZE, CUBE_DEFAULT_SIZE, CUBE_DEFAULT_SIZE))

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

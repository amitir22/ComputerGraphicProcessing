#pragma once
#include "scene.h"
#include "vec.h"
#include "mat.h"
#include <string>

using namespace std;

class MeshModel : public Model
{
protected :
	MeshModel() {}
	vec3 *vertex_positions;
	//add more attributes
	mat4 _world_transform; // also known as model transform
	mat3 _normal_transform;

public:

	MeshModel(string fileName);
	~MeshModel(void);
	void loadFile(string fileName);
	void draw();
	void translate(vec3 translation);
	void rotate(vec3 axis, float angle);
	void scale(vec3 scale);

	
};

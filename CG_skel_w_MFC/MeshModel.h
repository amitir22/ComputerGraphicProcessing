// MeshModel.h
#pragma once
#include "vec.h"
#include "mat.h"
#include "Renderer.h"
#include <vector>
#include <string>
#include <memory>

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
	std::vector<vec3> vertex_positions;
	std::vector<vec3> normal_positions;

	mat4 _model_transform; // also known as model transform
	mat3 _normal_transform;

	MeshModel() noexcept;
	explicit MeshModel(string fileName);
	void loadFile(string fileName);
	void draw(Renderer& renderer);
	// User Transformation
	void translate(vec3 translation);
	void rotate(vec3 axis, float angle);
	void scale(vec3 scale);
};

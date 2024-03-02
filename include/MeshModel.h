// MeshModel.h
#pragma once
#include "scene.h"
#include "vec.h"
#include "mat.h"
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

	FaceIdcs(std::istream& a_stream)
	{
		for (int i = 0; i < 4; i++)
			v[i] = vn[i] = vt[i] = 0;

		char c;
		for (int i = 0; i < 3; i++)
		{
			a_stream >> std::ws >> v[i] >> std::ws;
			if (a_stream.peek() != '/')
				continue;
			a_stream >> c >> std::ws;
			if (a_stream.peek() == '/')
			{
				a_stream >> c >> std::ws >> vn[i];
				continue;
			}
			else
				a_stream >> vt[i];
			if (a_stream.peek() != '/')
				continue;
			a_stream >> c >> vn[i];
		}
	}
};


class MeshModel : public Model
{
protected:
	std::vector<vec3> vertex_positions;
	std::vector<vec3> normal_positions;

	mat4 model_transform_; // also known as model transform
	mat3 normal_transform_;

public:
	MeshModel() noexcept;
	explicit MeshModel(string fileName);
	void LoadFile(string fileName);
	void Draw(Renderer& renderer);
	// User Transformation
	void Translate(vec3 translation);
	void Rotate(vec3 axis, float angle);
	void Scale(vec3 scale);


};

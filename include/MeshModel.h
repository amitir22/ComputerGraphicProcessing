// MeshModel.h
#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "Geometry.h"

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


class MeshModel
{
protected:
	Eigen::MatrixXf vertices_local_;
	Eigen::MatrixXf normals_local_;
	Eigen::MatrixXf face_normals_local_;
	Eigen::MatrixXf faces_midpoints_local_;

	mat4 model_transform_; // also known as model transform

public:
	MeshModel() noexcept;

	explicit MeshModel(string fileName);
	void LoadFile(string fileName);
	// User Transformations
	void Translate(vec3 translation);
	void Rotate(vec3 axis, float angle);
	void Scale(vec3 scale);
	
	mat4 GetModelTransform() const {return model_transform_;}
	// Getters for read-only access to the model's data
	const Eigen::MatrixXf& GetVerticesLocal() const {return vertices_local_;}

	const Eigen::MatrixXf& GetNormalsLocal() const {return normals_local_;}

	const Eigen::MatrixXf& GetFaceNormalsLocal() const {return face_normals_local_;}

	const Eigen::MatrixXf& GetFacesMidpointsLocal() const {return faces_midpoints_local_;}
};

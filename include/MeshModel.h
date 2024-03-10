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
	unsigned short v[4];
	unsigned short vn[4];
	unsigned short vt[4];

	FaceIdcs()
	{
		for (int i = 0; i < 4; i++)
			v[i] = vn[i] = vt[i] = 0;
	}

	FaceIdcs(std::istream& a_stream)
	{
		for (int i = 0; i < 4; ++i)
			v[i] = vn[i] = vt[i] = 0;

		char c; // For reading the '/' delimiter
		for (int i = 0; i < 3; ++i)
		{
			a_stream >> std::ws >> v[i]; // Read vertex index

			if (a_stream.peek() == '/')
			{
				a_stream >> c; // Consume '/'

				if (a_stream.peek() == '/') // Case: v//vn
				{
					a_stream >> c >> vn[i]; // Consume second '/' and read vn
				}
				else // Case: v/vt or v/vt/vn
				{
					a_stream >> vt[i]; // Read vt

					if (a_stream.peek() == '/')
					{
						a_stream >> c >> vn[i]; // Consume '/' and read vn
					}
				}
			}
		}
	}
};


class MeshModel
{
protected:
	string model_name_;
	Eigen::MatrixXf vertices_local_;
	Eigen::MatrixXf v_normals_local_;
	Eigen::MatrixXf face_normals_local_;
	Eigen::MatrixXf faces_midpoints_local_;
	// Bounding Box
	geometry::Box bounding_box_;

	mat4 model_transform_; // also known as model transform

public:
	MeshModel() noexcept;

	explicit MeshModel(string fileName);
	void SetModelName(string file_name);
	void LoadFile(string fileName);
	void ComputeBoundingBox();
	// User Transformations
	void Translate(vec3 translation);
	void Rotate(vec3 axis, float angle);
	void Scale(vec3 scale);
	
	mat4 GetModelTransform() const {return model_transform_;}
	// Getters for read-only access to the model's data
	const Eigen::MatrixXf& GetVerticesLocal() const {return vertices_local_;}

	const Eigen::MatrixXf& GetNormalsLocal() const {return v_normals_local_;}

	const Eigen::MatrixXf& GetFaceNormalsLocal() const {return face_normals_local_;}

	const Eigen::MatrixXf& GetFacesMidpointsLocal() const {return faces_midpoints_local_;}
};

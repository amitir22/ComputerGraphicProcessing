// Scene.h
#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "MeshModel.h"
#include "Renderer.h"
#include "Camera.h"
#include "mat.h"
using namespace std;


class Light {

};


class Scene {
public:
	std::vector<std::shared_ptr<MeshModel>> models;
	std::vector<std::shared_ptr<Light>> lights;
	std::vector<std::shared_ptr<Camera>> cameras;
	Renderer* m_renderer;
	// Constructors
	Scene() {};
	Scene(Renderer* renderer);
	
	void loadOBJModel(string fileName);
	void draw();
	void handleWindowReshape(int width, int height);
	void drawDemo();
	Camera* getActiveCamera() { return cameras[activeCamera].get(); }
	MeshModel* getActiveModel() { return models[activeModel].get(); }
	void applyTransformation(mat4 transformation) { models[activeModel]->applyTransformation(transformation);}
	
	int activeCamera;
	int activeModel;
	int activeLight;

	// View options
	bool isShowVertexNormals = false;
	bool isShowFaceNormals = false;
	bool isShowBoundingBox = false;
};
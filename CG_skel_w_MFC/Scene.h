// Scene.h
#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "Renderer.h"
#include "Camera.h"
#include "mat.h"
using namespace std;

class Model {
public:
	virtual ~Model() {}
	void virtual draw(Renderer& renderer)=0;
};


class Light {

};


class Scene {
	std::vector<std::unique_ptr<MeshModel>> models;
	std::vector<std::unique_ptr<Light>> lights;
	std::vector<std::unique_ptr<Camera>> cameras;
	Renderer* m_renderer;
public:
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
	
};
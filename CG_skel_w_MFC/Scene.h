#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "Renderer.h"
#include "Camera.h"
using namespace std;

class Model {
public:
	virtual ~Model() {}
	void virtual draw()=0;
};


class Light {

};


class Scene {

	vector<Model*> models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer *m_renderer;


public:
	Scene() {};
	Scene(Renderer *renderer) : m_renderer(renderer) {};
	void loadOBJModel(string fileName);
	void draw();
	void drawDemo();
	
	int activeModel;
	int activeLight;
	int activeCamera;
};
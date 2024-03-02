// Scene.h
#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "Camera.h"
#include "PathConfig.h" // for RESOURCE_DIR


class Model {
public:
	virtual ~Model() {}
	void virtual Draw(Renderer& renderer) = 0;
};


class Light {

};


class Scene {
	std::vector<std::unique_ptr<Model>> models_;
	std::vector<std::unique_ptr<Camera>> cameras_;
	std::vector<std::unique_ptr<Light>> lights_;
	Renderer* renderer_;
public:
	// Constructors
	Scene() {};
	Scene(Renderer* renderer);

	void LoadOBJModel(std::string file_name);
	void Draw();
	void DrawDemo();
	Camera* GetActiveCamera() { return cameras_[active_camera_idx].get(); }


	int active_camera_idx;
	int active_model_idx;
	int active_light_idx;

};
// Scene.h
#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Light.h"
#include "MeshModel.h"
#include "PathConfig.h" // for RESOURCE_DIR

class Scene {
	std::vector<std::unique_ptr<MeshModel>> models_;
	std::vector<std::unique_ptr<Camera>> cameras_;
	std::vector<std::unique_ptr<Light>> lights_;

public:
	int active_camera_idx;
	int active_model_idx;
	int active_light_idx;

	// Constructors
	Scene();

	void LoadOBJModel(std::string file_name);
    std::vector<MeshModel*>  GetModels();
	std::vector<Camera*> GetCameras();
    Camera* GetActiveCamera() { return cameras_[active_camera_idx].get(); }
};
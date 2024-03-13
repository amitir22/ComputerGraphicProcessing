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
	std::shared_ptr<AmbientLight> ambient_light_;

public:
	int active_camera_idx_;
	int active_model_idx_;
	int active_light_idx;

	// Constructors
	Scene();
	// Model
	void LoadOBJModel(std::string file_name);
    std::vector<MeshModel*>  GetModels();
	MeshModel* GetModel(int idx);
	std::vector<std::string> GetModelsNames();
	void DeleteMeshModel(int idx);
	MeshModel* GetActiveModel();
	// Light
	Light* GetLight(int idx);
	void DeleteLight(int idx);
	std::vector<Light*> GetLights();
	AmbientLight* GetAmbientLight();
	void AddPointLight(vec3 translation);
	// Camera
	std::vector<Camera*> GetCameras();
	Camera* GetActiveCamera();
};
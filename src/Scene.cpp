// Scene.cpp
#include "Scene.h"
#include "MeshModel.h"


using namespace std;

// Constructors
Scene::Scene() {
	// create a default camera
	cameras_.push_back(std::make_unique<Camera>());
	active_camera_idx = 0;
	active_model_idx = 0;
	active_light_idx = 0;
	// TODO delete, load from menu
	//LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/bunny.obj");
	//LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/banana.obj");
	//LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/cow.obj");
	//LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/triangle_tilt.obj");
	//LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/triangle_tilt_flipped.obj");
	//LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/two_triangles.obj");
	LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/trig_pyramid.obj");
	//LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/cube.obj");
	//LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/solids.obj");
}



void Scene::LoadOBJModel(string fileName)
{
	models_.push_back(std::make_unique<MeshModel>(fileName));
	active_model_idx = models_.size() - 1;
}

std::vector<MeshModel*> Scene::GetModels() {
	std::vector<MeshModel*> models;
	for (const auto& model : models_) {
		models.push_back(model.get());
	}
	return models;
}

std::vector<Camera*> Scene::GetCameras()
{
	std::vector<Camera*> cameras;
	for (const auto& camera : cameras_) {
		cameras.push_back(camera.get());
	}
	return cameras;
}

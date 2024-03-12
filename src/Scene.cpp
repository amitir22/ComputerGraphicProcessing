// Scene.cpp
#include "Scene.h"
#include "MeshModel.h"

using namespace std;

// Constructors
Scene::Scene() {
	// create a default camera
	cameras_.push_back(std::make_unique<Camera>());
	lights_.push_back(std::make_unique<PointLight>());
	lights_.push_back(std::make_unique<ParallelLight>(DEFAULT_LIGHT_INTENSITY, DEFAULT_LIGHT_COLOR, vec3(0, 1, 0)));
	ambient_light_ = make_shared<AmbientLight>();
	active_camera_idx_ = 0;
	active_model_idx_ = -1;
	active_light_idx = 0;
	// TODO delete, load from menu
	LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/bunny.obj");
	//LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/banana.obj");
	//LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/cow.obj");
	//LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/triangle_tilt.obj");
	//LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/triangle_tilt_flipped.obj");
	//LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/two_triangles.obj");
	//LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/trig_pyramid.obj");
	//LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/cube.obj");
	//LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/solids.obj");
}



void Scene::LoadOBJModel(string fileName)
{
	models_.push_back(std::make_unique<MeshModel>(fileName));
	active_model_idx_ = models_.size() - 1;
}

std::vector<MeshModel*> Scene::GetModels() {
	std::vector<MeshModel*> models;
	for (const auto& model : models_) {
		models.push_back(model.get());
	}
	return models;
}

MeshModel* Scene::GetModel(int idx)
{
	if (idx >= 0 && idx < models_.size()) {
		return models_[idx].get();
	}
	return nullptr;
}

std::vector<std::string> Scene::GetModelsNames()
{
	std::vector<std::string> model_names;
	for (const auto& model : models_) {
		model_names.push_back(model->GetModelName());
	}
	return model_names;
}

void Scene::DeleteMeshModel(int idx)
{
	if (models_.size() == 0) {
		return;
	}
	models_.erase(models_.begin() + idx);
	// update active model index accordingly. If deleted model was the active one, set active_model_idx_ to be new size minus 1
	if (active_model_idx_ == idx) {
		active_model_idx_ = models_.size() - 1;
	}
	else if (active_model_idx_ > idx) {
		active_model_idx_--;
	}

}

std::vector<Camera*> Scene::GetCameras()
{
	std::vector<Camera*> cameras;
	for (const auto& camera : cameras_) {
		cameras.push_back(camera.get());
	}
	return cameras;
}

std::vector<Light*> Scene::GetLights()
{
	std::vector<Light*> lights;
	for (const auto& light : lights_) {
		lights.push_back(light.get());
	}
	return lights;
}

AmbientLight* Scene::GetAmbientLight()
{
	return this->ambient_light_.get();
}

MeshModel* Scene::GetActiveModel()
{
	if (active_model_idx_ >= 0 && active_model_idx_ < models_.size())
		return models_[active_model_idx_].get();
	return nullptr;
}

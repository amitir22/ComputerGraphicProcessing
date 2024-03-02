// Scene.cpp
#include "Scene.h"
#include "MeshModel.h"


using namespace std;

// Constructors
Scene::Scene(Renderer* renderer) : renderer_(renderer) {
	// create a default camera
	cameras_.push_back(std::make_unique<Camera>());
	active_camera_idx = 0;
	active_model_idx = 0;
	active_light_idx = 0;
	LoadOBJModel(std::string(RESOURCES_DIR) + "/obj_examples/bunny.obj");
}

void Scene::LoadOBJModel(string fileName)
{
	models_.push_back(std::make_unique<MeshModel>(fileName));
	active_model_idx = models_.size() - 1;
}

void Scene::Draw()
{
	renderer_->ClearBuffers();
	// 1. Send the renderer the active camera transform and the projection
	Camera* active_cam = cameras_[active_camera_idx].get();
	renderer_->SetViewTransform(active_cam->GetCameraTransform());
	renderer_->SetProjectionTransform(active_cam->projection, active_cam->IsPerspectiveProjection());

	// 2. Tell all models to draw themselves
	for (auto& model : models_)
	{
		model->Draw(*renderer_);
	}
}
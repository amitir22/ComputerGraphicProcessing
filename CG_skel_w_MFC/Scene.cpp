// Scene.cpp
#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>
#include <memory>

using namespace std;

// Constructors
Scene::Scene(Renderer* renderer) : m_renderer(renderer) {
	// create a default camera
	cameras.push_back(std::make_shared<Camera>());
	activeCamera = 0;
	activeModel = 0;
	activeLight = 0;
	//loadOBJModel("..\\obj_examples\\trigPyramid.obj");
	//loadOBJModel("..\\obj_examples\\Bunny.obj");
}

void Scene::loadOBJModel(string fileName)
{
	models.push_back(std::make_shared<MeshModel>(fileName));
	activeModel = models.size() - 1;
}

void Scene::draw()
{
	m_renderer->ClearColorBuffer();
	// 1. Send the renderer the active camera transform and the projection
	Camera* activeCam = cameras[activeCamera].get();
	m_renderer->SetViewTransform(activeCam->getCameraTransformMatrix());
	m_renderer->SetProjection(activeCam->projection, activeCam->isPerspectiveProjection());
		

	// 2. Tell all models to draw themselves
	for (auto& model : models)
	{
		model->draw(*m_renderer, isShowVertexNormals);

		if (this->isShowBoundingBox)
		{
			model->drawBoundingBox(*m_renderer);
		}
		if (this->isShowFaceNormals)
		{
			model->drawFaceNormals(*m_renderer);
		}
		if (this->isShowVertexNormals)
		{
			model->drawVertexNormals(*m_renderer);
		}
	}

	m_renderer->SwapBuffers();
}

void Scene::handleWindowReshape(int width, int height)
{
	for (auto& camera : cameras)
	{
		camera->handleWindowReshape(width, height);
	}
}

void Scene::drawDemo()
{
	m_renderer->SetDemoBuffer();
	m_renderer->SwapBuffers();
}

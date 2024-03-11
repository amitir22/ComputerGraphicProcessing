// UI.h
#pragma once
#include "imgui.h"
#include <imfilebrowser.h>

#include "Renderer.h"
#include "PathConfig.h" // for RESOURCES_DIR

class UI {
public:
	Scene* scene_;
	Renderer* renderer_;
	ImGui::FileBrowser file_dialog_;
	bool is_model_list_window_open_ = true;

	UI(Scene* scene, Renderer* renderer);
	void ShowFileMenu();
	void ShowUI();
	void ShowModelListWindow();
};


// UI.h
#pragma once
#include "imgui.h"
#include <imfilebrowser.h>
#include "Constants.h"
#include "Renderer.h"
#include "PathConfig.h" // for RESOURCES_DIR

class UI {
public:
	Scene* scene_;
	Renderer* renderer_;
	ImGui::FileBrowser file_dialog_;
	bool is_model_list_window_open_ = true;
	bool show_color_picker_models = false;
	bool show_color_picker_lights = false;
	bool is_ligth_list_window_open_ = true;
	bool openAddPointLightModal = false;

	UI(Scene* scene, Renderer* renderer);
	void ShowFileMenu();
	void ShowUI();
	void ShowModelListWindow();
	void ShowLightListWindow();
};


#include "UI.h"

UI::UI(Scene* scene, Renderer* renderer)
{
	scene_ = scene;
	renderer_ = renderer;
	file_dialog_.SetTitle("Add Model");
	file_dialog_.SetPwd(std::string(RESOURCES_DIR) + "/obj_examples/");
	file_dialog_.SetTypeFilters({ ".obj" });
}

void UI::ShowFileMenu() {
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Import Mesh"))
        {
            file_dialog_.Open();
        }
        ImGui::EndMenu();
    }
    file_dialog_.Display();

    if (file_dialog_.HasSelected())
    {
        std::string file_full_path = file_dialog_.GetSelected().string(); 
        scene_->LoadOBJModel(file_full_path);
        file_dialog_.ClearSelected();
    }
}

void UI::ShowUI()
{
    ImGui::BeginMainMenuBar(); // Start the main menu bar
    ShowFileMenu();
    
    if (ImGui::BeginMenu("Display"))
    {
        ImGui::MenuItem("Vertex Normals", "", &(renderer_->show_vertex_normals_));
        ImGui::MenuItem("Face Normals", "", &(renderer_->show_face_normals_));
        ImGui::MenuItem("Cameras", "", &(renderer_->show_cameras_));
        ImGui::MenuItem("Lights", "", &(renderer_->show_lights_));
        ImGui::MenuItem("Wireframe", "", &(renderer_->show_wireframe_));
        ImGui::MenuItem("Bounding Box", "", &(renderer_->show_bounding_box_));
        ImGui::MenuItem("Backface Culling", "", &(renderer_->is_backface_culling_));
        ImGui::MenuItem("Clipping", "", &(renderer_->is_clipping_));
        ImGui::EndMenu();
    }
    
    if (ImGui::BeginMenu("Shading Type"))
    {
        if (ImGui::MenuItem("White", NULL, renderer_->selected_shading_type == WHITE)) {
            renderer_->selected_shading_type = WHITE;
        }
        if (ImGui::MenuItem("Flat", NULL, renderer_->selected_shading_type == FLAT)) {
            renderer_->selected_shading_type = FLAT;
        }
        if (ImGui::MenuItem("Gouraud", NULL, renderer_->selected_shading_type == GOURAUD)) {
            renderer_->selected_shading_type = GOURAUD;
        }
        if (ImGui::MenuItem("Phong", NULL, renderer_->selected_shading_type == PHONG)) {
            renderer_->selected_shading_type = PHONG;
        }
        ImGui::EndMenu();
    }
    
    ImGui::EndMainMenuBar();
    
    ShowModelListWindow();
}

void UI::ShowModelListWindow() {
    if (ImGui::Begin("Model List", &is_model_list_window_open_)) {
        auto models_names = scene_->GetModelsNames();

        for (int i = 0; i < models_names.size(); ++i) {
            ImGui::PushID(i); // Ensure unique ID for buttons within the loop
            // Check if this model is currently selected
            bool is_selected = (scene_->active_model_idx_ == i);
            // Selectable row for each model. When clicked, it selects the model.
            if (ImGui::Selectable(models_names[i].c_str(), is_selected)) {
                if (is_selected) {
                    // If the model was already selected, deselect it
                    scene_->active_model_idx_ = -1;
                }
                else {
                    // If the model was not already selected, select it
                    scene_->active_model_idx_ = i;
                }
            }

            ImGui::SameLine();

            // 'X' button for deleting the model
            if (ImGui::Button("X")) {
                scene_->DeleteModel(i);
            }

            ImGui::PopID();
        } // End of for loop
    } // End of window
    ImGui::End();
}

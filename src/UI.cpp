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
        ImGui::MenuItem("Vertex Normals", "", &(renderer_->draw_vertex_normals_));
        ImGui::MenuItem("Face Normals", "", &(renderer_->draw_face_normals_));
        ImGui::MenuItem("Cameras", "", &(renderer_->draw_cameras_));
        ImGui::MenuItem("Lights", "", &(renderer_->draw_lights_));
        ImGui::MenuItem("Wireframe", "", &(renderer_->draw_wireframe_));
        ImGui::MenuItem("Bounding Box", "", &(renderer_->draw_bounding_box_));
        ImGui::MenuItem("Backface Culling", "", &(renderer_->is_backface_culling_));
        ImGui::MenuItem("Clipping", "", &(renderer_->is_clipping_));
        ImGui::MenuItem("Axes", "", &(renderer_->show_axes_));
        ImGui::MenuItem("Bloom", "", &(renderer_->draw_bloom_));
        ImGui::EndMenu();
    }
    
    if (ImGui::BeginMenu("Shading Type"))
    {
        if (ImGui::MenuItem("White", NULL, renderer_->selected_shading_type == WHITE_SHADING)) {
            renderer_->selected_shading_type = WHITE_SHADING;
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
            // Start row for model name and buttons
            ImGui::BeginGroup();
            bool is_selected = (scene_->active_model_idx_ == i);
            if (ImGui::Selectable(models_names[i].c_str(), is_selected, ImGuiSelectableFlags_AllowDoubleClick)) {
                scene_->active_model_idx_ = is_selected ? -1 : i;
            }
            ImGui::EndGroup();

            ImGui::SameLine();

            // 'X' button for deleting the model
            if (ImGui::Button("X")) {
                scene_->DeleteMeshModel(i);
            }

            ImGui::SameLine();

            if (ImGui::Button("M")) {
                // Open a new window for model transformation
                ImGui::OpenPopup("Model Transformation");
                scene_->active_model_idx_ = i; // Assume this is an integer in your UI class indicating which model is being edited
            }

            if (ImGui::BeginPopup("Model Transformation")) {
                MeshModel* model = scene_->GetModel(scene_->active_model_idx_);
                if (model) {
                    vec3 translation = model->GetTranslation();
                    vec3 rotation = model->GetRotation();
                    vec3 scale = model->GetScale();

                    if (ImGui::InputFloat3("Translation", translation.data())) {
                        model->SetTranslation(translation);
                    }
                    if (ImGui::InputFloat3("Scale", scale.data())) {
                        model->SetScale(scale);
                    }
                    if (ImGui::InputFloat3("Rotation", rotation.data())) {
                        model->SetRotation(rotation); // Assuming pitch, yaw, roll order
                    }
                }
                ImGui::EndPopup();
            }
            ImGui::SameLine();

            if (ImGui::Button("Color")) {
                show_color_picker_ = true; // Toggle visibility instead of directly opening the picker
            }
            if (show_color_picker_) 
            {   
                ImVec4 current_color = ImVec4(0, 0, 0, 1);
                // Convert your vec3 color to ImVec4, ImGui uses ImVec4 for colors
                vec3 color_vec = this->scene_->GetActiveModel()->GetUniformMaterialColor();
                current_color.x = color_vec.x(); // R
                current_color.y = color_vec.y(); // G
                current_color.z = color_vec.z(); // B

                if (ImGui::Begin("Color Picker", &show_color_picker_)) {
                    if (ImGui::ColorPicker3("Material Color", (float*)&current_color)) {
                        // When the color is picked, set the new color
                        this->scene_->GetActiveModel()->SetUniformMaterialColor(vec3(current_color.x, current_color.y, current_color.z));
                    }
                    ImGui::End();
                }
            }

            ImGui::PopID();
        } // End of for loop
    } // End of window
    ImGui::End();
}

// CG.cpp : Defines the entry point for the application.
#include <iostream>
#include <string>

#include <Eigen/Dense>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imfilebrowser.h>


#include "Callbacks.h"
#include "Constants.h"
#include "ControlState.h"
#include "PathConfig.h" // for RESOURCES_DIR
#include "Shader.h"
#include "Renderer.h"

Renderer* renderer;
Scene* scene;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(cg::constants::SCR_WIDTH, cg::constants::SCR_HEIGHT, "CG", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    ControlState control_state(cg::constants::SCR_WIDTH, cg::constants::SCR_HEIGHT);
    glfwSetWindowUserPointer(window, &control_state);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    Shader shader_program((std::string(RESOURCES_DIR) + "/shaders/shader.vs").c_str(), (std::string(RESOURCES_DIR) + "/shaders/shader.fs").c_str());

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions   // texture coords
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,

        -1.0f,  1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Generate and bind a texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // Set texture wrapping and filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Allocate texture 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cg::constants::SCR_WIDTH, cg::constants::SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    // Prepare data for texture
    scene = new Scene();
    renderer = new Renderer(cg::constants::SCR_WIDTH, cg::constants::SCR_HEIGHT);

    // Dear ImGui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    // create a file browser instance
    ImGui::FileBrowser fileDialog;

    // (optional) set browser properties
    fileDialog.SetTitle("title");
    //fileDialog.SetTypeFilters({ ".h", ".cpp" });
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        control_state.UpdateDeltaTime(static_cast<float>(glfwGetTime()));
        // input
        // -----
        ProcessInput(window);
        renderer->DrawScene(scene);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, renderer->width_, renderer->height_, GL_RGB, GL_UNSIGNED_BYTE, renderer->framebuffer_.get());

        // Render
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Render container
        shader_program.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //if (ImGui::Begin("dummy window")) { // open file dialog when user clicks this button
        //    if (ImGui::Button("open file dialog"))
        //        fileDialog.Open(); }
        //ImGui::End();
        //fileDialog.Display();
        //if (fileDialog.HasSelected()) {
        //    std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
        //    fileDialog.ClearSelected();}

        ImGui::BeginMainMenuBar(); // Start the main menu bar

        if (ImGui::BeginMenu("Display")) {
            ImGui::MenuItem("Vertex Normals", "", &(renderer->show_vertex_normals_));
            ImGui::MenuItem("Face Normals", "", &(renderer->show_face_normals_));
            ImGui::MenuItem("Cameras", "", &(renderer->show_cameras_));
            ImGui::MenuItem("Lights", "", &(renderer->show_lights_));
            ImGui::MenuItem("Wireframe", "", &(renderer->show_wireframe_));
            ImGui::MenuItem("Bounding Box", "", &(renderer->show_bounding_box_));
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // GLFW: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    // delete renderer and scene
    delete renderer;
    delete scene;
    return 0;
}


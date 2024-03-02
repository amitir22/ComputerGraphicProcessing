// Callbacks.h
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Scene.h"

extern Scene* scene;


void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);
void MouseCallback(GLFWwindow* window, double x_pos_in, double y_pos_in);
void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset);
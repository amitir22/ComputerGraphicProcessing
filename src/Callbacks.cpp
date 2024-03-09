#include "Callbacks.h"

#include "Constants.h" // for SCR_WIDTH, SCR_HEIGHT
#include "ControlState.h" // for ControlState


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void ProcessInput(GLFWwindow* window)
{
    ControlState* control_state = static_cast<ControlState*>(glfwGetWindowUserPointer(window));
    Camera* active_camera = scene->GetActiveCamera();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        active_camera->HandleKeyboardInput(FORWARD, control_state->delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        active_camera->HandleKeyboardInput(BACKWARD, control_state->delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        active_camera->HandleKeyboardInput(LEFT, control_state->delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        active_camera->HandleKeyboardInput(RIGHT, control_state->delta_time);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        active_camera->HandleKeyboardInput(UP, control_state->delta_time);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        active_camera->HandleKeyboardInput(DOWN, control_state->delta_time);
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        active_camera->HandleKeyboardInput(ORBIT_L, control_state->delta_time);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        active_camera->HandleKeyboardInput(ORBIT_R, control_state->delta_time);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void MouseCallback(GLFWwindow* window, double x_pos_in, double y_pos_in) 
{
    ControlState* control_state = static_cast<ControlState*>(glfwGetWindowUserPointer(window));
    Camera* active_camera = scene->GetActiveCamera();

    if (control_state->first_mouse)
    {
        control_state->last_x = x_pos_in;
        control_state->last_y = y_pos_in;
        control_state->first_mouse = false;
    }

    float x_offset = x_pos_in - control_state->last_x;
    float y_offset = control_state->last_y - y_pos_in; // reversed since y-coordinates go from bottom to top

    control_state->last_x = x_pos_in;
    control_state->last_y = y_pos_in;

    active_camera->HandleMouseMovement(x_offset, y_offset);
}

void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset)
{
    Camera* active_camera = scene->GetActiveCamera();
    active_camera->HandleMouseScroll(static_cast<float>(y_offset));
}


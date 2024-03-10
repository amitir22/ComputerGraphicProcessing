#include "Callbacks.h"

#include "Constants.h" // for SCR_WIDTH, SCR_HEIGHT
#include "ControlState.h" // for ControlState


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ControlState* control_state = static_cast<ControlState*>(glfwGetWindowUserPointer(window));
    Camera* active_camera = scene->GetActiveCamera();

    // Handle left control key state update
    if (key == GLFW_KEY_LEFT_CONTROL) {
        control_state->ctrl_pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_W:
            active_camera->Translate(FORWARD, control_state->delta_time);
            break;
        case GLFW_KEY_S:
            active_camera->Translate(BACKWARD, control_state->delta_time);
            break;
        case GLFW_KEY_A:
            active_camera->Translate(LEFT, control_state->delta_time);
            break;
        case GLFW_KEY_D:
            active_camera->Translate(RIGHT, control_state->delta_time);
            break;
        case GLFW_KEY_Q:
            active_camera->Translate(UP, control_state->delta_time);
            break;
        case GLFW_KEY_E:
            active_camera->Translate(DOWN, control_state->delta_time);
            break;
        case GLFW_KEY_R:
            active_camera->Reset();
            break;
        }
    }

    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void MousePosCallback(GLFWwindow* window, double x_pos_in, double y_pos_in) 
{
    ControlState* control_state = static_cast<ControlState*>(glfwGetWindowUserPointer(window));

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
    if (control_state->left_mouse_pressed) {
		Camera* active_camera = scene->GetActiveCamera();
		active_camera->Orbit(x_offset, y_offset);
	}    
}

void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset)
{
    Camera* active_camera = scene->GetActiveCamera();
    active_camera->HandleMouseScroll(static_cast<float>(y_offset));
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    ControlState* control_state = static_cast<ControlState*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        control_state->left_mouse_pressed = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        control_state->right_mouse_pressed = (action == GLFW_PRESS);
	}
}


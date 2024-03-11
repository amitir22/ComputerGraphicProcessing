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
    // if key is shift, update the state of the shift key
    if (key == GLFW_KEY_LEFT_SHIFT) {
        control_state->shift_pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_R:
            active_camera->Reset();
            break;
        case GLFW_KEY_O:
            active_camera->OrbitLeft();
            break;
        case GLFW_KEY_P:
            active_camera->OrbitRight();
            break;
        }
    }
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    width = height * 4.0 / 3.0;
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    renderer->HandleWindowReshape(width, height);
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
    Camera* active_camera = scene->GetActiveCamera();
    if (control_state->left_mouse_pressed) {
        if (control_state->ctrl_pressed) {
            active_camera->Pan(-x_offset, -y_offset);
        }
        else {
            active_camera->Orbit(-x_offset, y_offset);
        }
	}
    else if (control_state->right_mouse_pressed) {
        std::cout << "x_offset: " << x_offset << " y_offset: " << y_offset << "\n";
        active_camera->TiltAndYaw(-x_offset, y_offset);
		
    }
}

void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset)
{
    ControlState* control_state = static_cast<ControlState*>(glfwGetWindowUserPointer(window));
    Camera* active_camera = scene->GetActiveCamera();
    // if shift is pressed, dolly the camera
    if (control_state->shift_pressed) {
		active_camera->Dolly(static_cast<float>(y_offset));
	} 
    else { // zoom
        active_camera->Zoom(static_cast<float>(y_offset));
    }
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


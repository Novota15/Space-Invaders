// control functions
#include "controls.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods, int move_dir, bool fire_pressed, bool game_running) {
    switch (key) {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS) game_running = false;
        break;
    case GLFW_KEY_RIGHT:
        if (action == GLFW_PRESS) move_dir += 1;
        else if(action == GLFW_RELEASE) move_dir -= 1;
        break;
    case GLFW_KEY_LEFT:
        if (action == GLFW_PRESS) move_dir -= 1;
        else if (action == GLFW_RELEASE) move_dir += 1;
        break;
    case GLFW_KEY_SPACE:
        if (action == GLFW_RELEASE) fire_pressed = true;
        break;
    default:
        break;
    }
    return;
}
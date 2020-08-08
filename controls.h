// game controls header
#ifndef CONTROLS_H
#define CONTROLS_H

#include "main.h"

// global vars
int move_dir = 0;
bool fire_pressed = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif
// game controls header
#ifndef CONTROLS_H
#define CONTROLS_H

#include "main.h"

// global vars
extern int move_dir;
extern bool fire_pressed;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif
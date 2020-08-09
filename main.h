// main header
#ifndef MULTITHREAD_H
#define MULTITHREAD_H

// standard C IO header
#include <cstdio>
#include <cstdint>
// include loading library
#include <GL/glew.h>
// include GLFW - lightweight opengl library
#include <GLFW/glfw3.h>
// numeric limits
#include <limits>

// global vars
extern bool game_running;
extern size_t aliens_killed;
extern size_t alien_update_timer;
extern size_t alien_update_frequency;
extern bool should_change_speed;
extern size_t score;
extern size_t credits;
extern int alien_move_dir;
extern int player_move_dir;
extern size_t alien_swarm_position;

void error_callback(int error, const char* description);

#endif
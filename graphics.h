// game graphics header
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "main.h"
#include "game-objects.h"

int initialize_glfw();

GLFWwindow* create_game_window(size_t buffer_width, size_t buffer_height);

void query_and_report_opengl_versions();

void buffer_draw_number(Buffer* buffer, const Sprite& number_spritesheet, size_t number, size_t x, size_t y, uint32_t color);

void buffer_draw_text(Buffer* buffer, const Sprite& text_spritesheet,const char* text, size_t x, size_t y, uint32_t color);

uint32_t rgb_to_uint32(uint8_t r, uint8_t g, uint8_t b);

#endif